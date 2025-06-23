# lvbfc (lv's Brainfuck compiler)

Brainfuck to C transpiler + compiler written in C, using the llv library.

## Features

| Feature                   | Description |
|---------------------------|-------------|
| Optimized Transpilation   | Converts Brainfuck into efficient, readable C |
| Operation Folding         | Collapses sequences like `++--`, `>><<` |
| Strict Loop Analysis      | Detects unsafe loops, warns/fails on invalid constructs |
| Dead Code Elimination     | Strips unused code and non-effect loops |
| Native Binary Output      | Outputs `.c` + native ELF via `gcc` or `clang` |
| Wraparound Control        | Circular buffer by default, can be disabled with `--no-wrap` |
| Heap Mode (`--heap`)      | Dynamic memory allocation for tape |
| Shellcode Mode (`--x`)    | Injects and runs syscalls via `;` sequences |
| Canary Debugging          | `?` inserts C comments, `??` dumps memory |
| Extended Pointer Ops      | `&`, `=` and similar are enabled in `--x` mode |
| Debug Stream Dump         | `--dmp-tok` shows token stream before emit |
| Clean C, Minimal Build    | Works with just `make`, `gcc` or `clang`, no dependencies |

## Build

```sh
make
```

>[!NOTE]
>running `make` will autofetch llv as it is it's own repo

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

  --heap            Use heap allocation for dynamic memory growth

  --x               Enable syscall shellcode emit mode (experimental)
                    Use `;` x N to mark syscall byte count

  --allow-canary    Enable `?` and `??` instructions

  --stacksize=N     Initial memory size in bytes (default: 65536)

  --opt-level=N     Optimization passes (default: 0) rm they don't
                    change a lot honestly

  --dmp-tok         Print parsed token stream (debugging only)

  --help            Show this help message and exit

Output:
  Produces a .c + native binary via GCC/Clang (fallback if needed).
  Output binary defaults to './bfout' unless specified.

Examples:
  ./lvbfc hello.b hello         # compile to ./hello
  ./lvbfc code.b --heap         # safer & dynamic memory growth
  ./lvbfc syscall.b --x         # run syscall via shellcode
  ./lvbfc file.b --stacksize=0  # invalid → error

Suggestions:
  - Fastest:             --no-wrap --opt-level=5
  - Mid‑range:           --heap
  - Shellcode mode:      --x with `;;;;;;` to exec 6‑byte syscall
  - Safe default:        no flags
```

## Custom Operators (Shellcode Mode: `--x`)

When compiling with `--x`, `lvbfc` enables a set of **non-standard extended operators** designed to support raw memory inspection, pointer manipulation, and direct shellcode injection.

These operators are **only active when `--x` is enabled**. They will be ignored or rejected in standard modes.

| Operator | Name                  | Description |
|----------|-----------------------|-------------|
| `;`      | Syscall Exec Marker   | Copies `N` bytes from the tape (starting at the current pointer) into an executable buffer and runs it. Supports AVX2/SSE if aligned, does no bounds checking. |
| `?`      | Canary Marker         | Inserts a debug comment in the generated C code. |
| `??`     | Buffer Dump           | Prints a hexadecimal dump of the memory from the start of the tape up to the current pointer. |
| `&`      | Pointer Writer        | Writes the address of `buf + (N - 1)` into the tape at the starting at the current cell (as a raw `uintptr_t`). Allows constructing absolute syscall arguments. Stackable: e.g. `&&&` writes address of `buf + 2` at `buf`, does no bounds checking. |
| `=`      | Pointer Jump          | Interprets the current cell as a pointer (raw `uintptr_t`) and sets `buf` to that address. Enables pointer-based control flow and tape indirection, does no bounds checking. |


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
string the debug stops at the current pos of the ptr

$ lvbfc helloworld.b --no-wrap --no-strict

WARNING: potential dangerous loop (no '-' in loop) detected @ seq. 24
[lvbfc] compiling
Optimized 0 ops
Collapsed from 106 total-length ops to 58 operations
[lvbfc] compiled successfully!

$ ./bfout

Hello World!

$ lvbfc helloworld.b --no-wrap --no-strict --allow-canary

WARNING: potential dangerous loop (no '-' in loop) detected @ seq. 24
[lvbfc] compiling
Optimized 0 ops
Collapsed from 108 total-length ops to 59 operations
[lvbfc] compiled successfully!

$ ./bfout

[CANARY] Dumping 7 bytes:
00 00 48 64 57 21 0A
Hello World!

```

## The 'Pointer' Feature

- The & instruction writes the raw address of a memory location into the current cell. Specifically, it stores the address of buf + (N - 1) as a uintptr_t value starting at the current tape cell. Multiple & instructions are stackable: each one increases N, and the resulting address overwrites the previous one.

- The = instruction jumps the internal pointer to the address currently stored at the tape's current position. This effectively sets buf to a new location, allowing arbitrary pointer redirection. The contents are interpreted as a raw uintptr_t, and no bounds checking is performed.

To test movement functionality compile `ptrs.b` in `tests` with --x --allow-canary.

```brainfuck
Pointer jumping test

->->->->->->->->->->->->->->->->->->->->-??
<<<<<<<<<<<<<<<<<<<<
&&&&
>>>>>>>>>>>>>>>>>>>>??
<<<<<<<<<<<<<<<<<<<<
=
??
```

Expected output:

```text
$ lvbfc --no-wrap  --no-strict ptrs.b --x --allow-canary

[lvbfc] compiling
[lvbfc] compiled successfully!

$ ./bfout
[CANARY] Dumping 21 bytes:
FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
[CANARY] Dumping 21 bytes:
03 30 EA 4F FF 7F 00 00 FF FF FF FF FF FF FF FF FF FF FF FF FF
[CANARY] Dumping 4 bytes:
03 30 EA 4F

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

First fill the bytes of the shellcode

>+++++++++[<++++++++++++++++++++>-]<++++
>>++++[<++++++++++>-]<->>>>>+++[<+++++>-]
+++++>>++++++++++++++++++++[<++++++++++>-]<-----

??

<<<<<<<    go back to the
           first byte of
           the syscall

;;;;;;;;   run a 8 byte syscall

```

This will literally evaluate to this:

```shell
$ ./bfout
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
// buffer movements
if (execbuf == MAP_FAILED) { perror("mmap failed"); exit(EXIT_FAILURE); }
__builtin_memcpy(execbuf, buf, 8);
((void(*)(void))execbuf)();
__builtin_memset(execbuf, 0, 8);
munmap(execbuf, 512);
return 0;
}
```

But in case we wanted a faster load, we can use more `;` than needed to match a multiple of 16 or 32, that way this:

```brainfuck
>+++++++++[<++++++++++++++++++++>-]<++++
>>++++[<++++++++++>-]<->>>>>+++[<+++++>-]
+++++>>++++++++++++++++++++[<++++++++++>-]<-----
<<<<<<<;;;;;;;;;;;;;;;;
```
Turns into this

```c
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <stdlib.h>

int main(void) {__attribute__((aligned(256))) uint8_t arr[65536] = {0};uint8_t *buf = &(arr[0]);
void *execbuf = mmap(NULL, 512, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
// buffer movements
if (execbuf == MAP_FAILED) { perror("mmap failed"); exit(EXIT_FAILURE); }
{
    size_t chunks = 16 / 16;
    for (size_t i = 0; i < chunks; ++i) {
        __m128i chunk = _mm_loadu_si128((const __m128i *)(buf + i * 16));
        _mm_storeu_si128((__m128i *)(execbuf + i * 16), chunk);
    }
}
((void(*)(void))execbuf)();
__builtin_memset(execbuf, 0, 16);
munmap(execbuf, 512);
return 0;
}
```


### Notes

* This shellcode will call `getpid()` and then return safely.
* Because the syscall has no output, you'll only see effects with strace or a debugger.

>[!NOTE]
> `getpid.b` can be found in `tests/`

## Requirements

- C compiler: GCC or Clang
- make

## Tests

Compliant with https://brainfuck.org/tests.b (included under tests/compliance) and includes various example programs.

## Benchmarks (`mandelbrot.b`)

Compiler                                       | Elapsed | Instructions   | Cycles        | IPC  | Branch Misses
----------------------------------------------|---------|----------------|---------------|------|----------------
lvbfc (--no-strict --no-wrap --opt-level=1)   | 0.589 s | 4.23B          | 2.64B         | 1.60 | 4.25 %
lvbfc (--no-strict --no-wrap --opt-level=0)   | 0.590 s | 4.24B          | 2.64B         | 1.64 | 4.30 %
lvbfc (--no-strict --heap --opt-level=1)      | 1.021 s | 11.61B         | 4.57B         | 2.54 | 1.98 %
[bfjitc -O3 (C)](https://github.com/tsoding/bfjit)     | 1.586 s | 5.54B          | 3.59B         | 1.54 | 4.00 %
[btc (LLVM)](https://github.com/Wilfred/bfc)                                            | 0.730 s | 5.97B          | 3.27B         | 1.83 | 4.15 %
Python (brainfuck.py)                          | 1830.571 s | 25.19T         | 5.95T         | 4.24 | 0.08 %

## TODO

- [ ] Multi-file compilation support

>[!WARNING]
> --opt-level=1 can mess with really complex programs like utm.b or bitwidth.b, use --opt-level=0 for these, it still does heavy opts, but not as aggressively, every other program works just fine. (It'll fail with the cc/clang command, so you'll know, i'm working on it, but i'm not a magician)
