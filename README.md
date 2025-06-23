# lvbfc (lv's Brainfuck compiler)

Brainfuck to C transpiler + compiler written in C, using the llv library.

## Features

- Converts Brainfuck code into optimized C
- Pattern optimizations (e.g., [-], M, C)
- Strict mode for loop sanity checks
- Operation cancellation/balancing
- Code sanitization, dead‑code + infinite‑loop removal
- Outputs native binaries
- Wrapping buffer mode with no undefined behavior (optional)
- Optional heap mode for dynamic memory
- Optional syscall injection mode from the current cursor up to N (via Brainfuck `;` count)

## Build

```sh
make
```

## Usage
```
lvbfc - Brainfuck compiler
---------------------------------------------------
Usage:
  ./lvbfc <input.b> [output] [options]

Arguments:
  <input.b>         Brainfuck source file (required)
  [output]          Output binary name (optional)
                    Default: bfout

Options:
  --no-strict       Disable safety checks for loops @ comptime

  --no-wrap         Disable buffer wraparound (executes faster)

  --opts            Use heap allocation for dynamic memory growth

  --x               Enable syscall shellcode emit mode (experimental)
                    Use `;` x N to mark syscall byte count

  --allow-canary    Enable `?` and `??` instructions

  --stacksize=N     Initial memory size in bytes (default: 65536)

  --opt-level=N     Optimization passes (default: 3)
                    More ≠ better (balance iteration vs comptime)

  --dmp-tok         Print parsed token stream (debugging only)

  --help            Show this help message and exit

Output:
  Produces a .c + native binary via GCC/Clang (fallback if needed).
  Output binary defaults to './bfout' unless specified.

Examples:
  ./lvbfc hello.b hello         # compile to ./hello
  ./lvbfc code.b --opts         # safer & dynamic memory growth
  ./lvbfc syscall.b --x         # run syscall via shellcode
  ./lvbfc file.b --stacksize=0  # invalid → error

Suggestions:
  - Fastest:             --no-wrap --opt-level=5
  - Mid‑range:           --opts --opt-level=5 (heap)
  - Shellcode mode:      --x with `;;;;;;` to exec 6‑byte syscall (the same as --opts)
  - Safe default:        no flags
```

## Requirements

- C compiler: GCC or Clang
- make

## Mandelbrot Benchmark

Compiler                                       | Elapsed | Instructions   | Cycles        | IPC  | Branch Misses
----------------------------------------------|---------|----------------|---------------|------|----------------
lvbfc (--no-strict --no-wrap --opt-level=5)   | 0.589 s | 4.23B          | 2.64B         | 1.60 | 4.25 %
lvbfc (--no-strict --opts --opt-level=5)      | 1.021 s | 11.61B         | 4.57B         | 2.54 | 1.98 %
[bfjitc](https://github.com/tsoding/bfjit)     | 1.586 s | 5.54B          | 3.59B         | 1.54 | 4.00 %
[btc](https://github.com/Wilfred/bfc)                                            | 0.730 s | 5.97B          | 3.27B         | 1.83 | 4.15 %
Python (brainfuck.py)                          | 1830.571 s | 25.19T         | 5.95T         | 4.24 | 0.08 %

## Tests

Compliant with https://brainfuck.org/tests.b (included under tests/compliance) and includes various example programs.

## The 'Canary' Feature

The `canary` feature, enabled by the `--allow-canary` option, introduces special debugging instructions into your Brainfuck code using the `?` character.

- A single `?` in your Brainfuck code will simply insert a comment in the generated C output, marking a 'CANARY' point. It's a visual cue for where you've placed a check.

- The `??` instruction, however, triggers a **memory buffer dump**. When encountered, the generated C code will:
    - Calculate the current size of the active memory buffer segment (from the beginning of the buffer to the current position).
    - Print a message to 'stderr' indicating the number of bytes being dumped.
    - Display the hexadecimal values of each byte in that memory segment, formatted for readability (40 bytes per line).

```text
$ cat helloworld.b

++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>
---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++??.

NOTE: this program doesn't end at the end of the
string, the debug stops at the current pos. of the ptr

$ lvbfc helloworld.b --no-wrap --no-strict

WARNING: potential dangerous loop (no '-' in loop) detected @ seq. 24
[lvbfc] compiling
Optimized 0 ops
Collapsed from 106 total-length ops to 58 operations
[lvbfc] compililed successfully!

$ ./bfout

Hello World!

$ lvbfc helloworld.b --no-wrap --no-strict --allow-canary

WARNING: potential dangerous loop (no '-' in loop) detected @ seq. 24
[lvbfc] compiling
Optimized 0 ops
Collapsed from 108 total-length ops to 59 operations
[lvbfc] compililed successfully!

$ ./bfout

[CANARY] Dumping 7 bytes:
00 00 48 64 57 21 0A
Hello World!

```

##  Syscall Mode Example: `getpid`

This example shows how to use the `--x` shellcode injection mode to perform a simple syscall. We inject raw x86-64 shellcode into executable memory and jump into it.

### Shellcode

```asm
mov eax, 39    ; syscall number for getpid
syscall
ret
````

### Bytes

Hex: b8 27 00 00 00 0f 05 c3

Decimal: 184, 39, 0, 0, 0, 15, 5, 195

### Example Brainfuck Program (`getpid.b`)

> This sets the memory buffer with the bytes above:

```brainfuck
NOTE: run with allow canary to see the
      values beign added in real time

++++++++++ 10
++++++++++ 20
++++++++++ 30
++++++++++ 40
++++++++++ 50
++++++++++ 60
++++++++++ 70
++++++++++ 80
++++++++++ 90
++++++++++ 100
++++++++++ 110
++++++++++ 120
++++++++++ 130
++++++++++ 140
++++++++++ 150
++++++++++ 160
++++++++++ 170
++++++++++ 180
++++       184
>          first byte set

??

++++++++++ 10
++++++++++ 20
++++++++++ 30
+++++++++  39
>          second byte set
>          third byte set
>          forth byte set
>          sixth byte set

??

++++++++++ 10
+++++      15
>          seventh byte set

??

+++++      5
>          eighth byte set

??

++++++++++ 10
++++++++++ 20
++++++++++ 30
++++++++++ 40
++++++++++ 50
++++++++++ 60
++++++++++ 70
++++++++++ 80
++++++++++ 90
++++++++++ 100
++++++++++ 110
++++++++++ 120
++++++++++ 130
++++++++++ 140
++++++++++ 150
++++++++++ 160
++++++++++ 170
++++++++++ 180
++++++++++ 190
+++++      195

??

<<<<<<<    go back to the
           first byte of
           the syscall

;;;;;;;;   run a 8 byte syscall

```

This will literally evaluate to this:

```shell
$ ./bfout
[CANARY] Dumping 2 bytes:
B8 00 
[CANARY] Dumping 6 bytes:
B8 27 00 00 00 00 
[CANARY] Dumping 7 bytes:
B8 27 00 00 00 0F 00 
[CANARY] Dumping 8 bytes:
B8 27 00 00 00 0F 05 00 
[CANARY] Dumping 8 bytes:
B8 27 00 00 00 0F 05 C3 
```

This (without --allow-canary for clarity) literally transpiles to:

```c
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <stdlib.h>

int main(void) {uint8_t arr[65536] = {0};
uint8_t *buf = &(arr[0]);
void *execbuf = mmap(NULL, 512, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
*buf += 184;
buf += 1;
*buf += 39;
buf += 4;
*buf += 15;
buf += 1;
*buf += 5;
buf += 1;
*buf += 195;
buf -= 7;
if (execbuf == MAP_FAILED) { perror("mmap failed"); exit(EXIT_FAILURE); }
__builtin_memcpy(execbuf, buf, 8);
((void(*)(uint8_t *))execbuf)(buf);
__builtin_memset(execbuf, 0, 8);
munmap(execbuf, 512);
return 0;
}
```

### Notes

* This shellcode will call `getpid()` and then return safely.
* Because the syscall has no output, you'll only see effects with strace or a debugger.

>[!NOTE]
> `getpid.b` can be found in `tests/`

## TODO

- [ ] Multi-file compilation support

>[!WARNING]
> --opt-level=1 can mess with really complex programs like utm.b or bitwidth.b, use --opt-level=0 for these, it still does heavy opts, but not as aggressively, every other program works just fine. (It'll fail with the cc/clang command, so you'll know, i'm working on it, but i'm not a magician)
