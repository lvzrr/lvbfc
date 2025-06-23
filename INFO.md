## Custom Operators (Shellcode Mode: `--x`)

When compiling with `--x`, `lvbfc` enables a set of **non-standard extended operators** designed to support raw memory inspection, pointer manipulation, and direct shellcode injection.

These operators are **only active when `--x` is enabled**. They will be ignored or rejected in standard modes.

| Operator | Name                  | Description |
|----------|-----------------------|-------------|
| `;`      | Syscall Exec Marker   | Copies `N` bytes from the tape (starting at the current pointer) into an executable buffer and runs it. Supports AVX2/SSE if aligned, does no bounds checking. |
| `?`      | Canary Marker         | Inserts a debug comment in the generated C code. |
| `??`     | Buffer Dump           | Prints a hexadecimal dump of the memory from the start of the tape up to the current pointer. |
| `&`      | Pointer Writer        | Writes the address of `buf + (N - 1)` into the tape, from the current cell (as a raw `uintptr_t`) to the current cell + N. Allows constructing absolute syscall arguments. Stackable: e.g. `&&&` writes address of `buf + 2` at `buf`, does bounds checking. |
| `=`      | Pointer Jump          | Interprets the current cell as a pointer (raw `uintptr_t`) and sets `buf` to that address. Enables pointer-based control flow and tape indirection, does bounds checking. |


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


## `$` Intrinsic Operations (Experimental)

When using the `$` operator, how many times `$` is repeated trigger special intrinsic instructions. These are non-standard Brainfuck extensions intended for advanced users.

| `$x`  | Description                                                                    | C Equivalent Code                                      |
|-------|--------------------------------------------------------------------------------|--------------------------------------------------------|
| `$1`  | Set the current cell to zero.                                                  | `*buf = 0;`                                            |
| `$2`  | Zero out `*buf` bytes starting at `buf`.                                       | `__builtin_memset(buf, 0, *buf);`                      |
| `$3`  | Set `*(buf + 1)` bytes at `buf` to the value in `*buf`.                             | `__builtin_memset(buf, *buf, *(buf + 1));`              |
| `$5`  | Multiply current cell by next cell (`*buf *= *(buf + 1);`).                    | `*buf *= *(buf + 1);`                                  |

Note:
- To use `$x`, simply repeat the `$` character `x` times (e.g., `$$$` for `$3`).
- These operations bypass traditional Brainfuck semantics and offer low-level memory manipulation capabilities.
- Use with caution and self-respect.

### Example Brainfuck Program (`propagateintr.b`)

This demonstrates `$3` setting a byte value and length via the tape,then executing the intrinsic to propagate that value across multiple cells.

```brainfuck
->>>>>>>>>>>>>??
<<<<<<<<<<<??
++++<??
+++++++??<
$$$
>>>>>>>>>>>>>??
````

This will literally produce:

```
$ ./bfout
[CANARY] Dumping 14 bytes:
FF 00 00 00 00 00 00 00 00 00 00 00 00 00
[CANARY] Dumping 3 bytes:
FF 00 00
[CANARY] Dumping 2 bytes:
FF 00
[CANARY] Dumping 2 bytes:
FF 07
[CANARY] Dumping 14 bytes:
FF FF FF FF FF FF FF 00 00 00 00 00 00 00
```

Where we propagate 0xFF across 7 cells, as the second-to-last canary

---

### Example Brainfuck Program (`memsetintr.b`)

This sets up a sequence of `0xFF` values, uses `$2` to clear a prefix of cells, and inspects memory before and after.

```brainfuck
>>->->->->->->->-??<<<<<<<<<++++++??$$>>>>>>>>>??
```

This will literally output:

```
$ ./bfout
[CANARY] Dumping 10 bytes:
00 00 FF FF FF FF FF FF FF FF
[CANARY] Dumping 1 bytes:
06
[CANARY] Dumping 10 bytes:
00 00 00 00 00 00 FF FF FF FF
```

Zeros out the cell and the concurrent N cells based on the value it points to when called

## Requirements

- C compiler: GCC or Clang
- make

## Tests

Compliant with https://brainfuck.org/tests.b (included under tests/compliance) and includes various example programs.

## Benchmarks (`mandelbrot.b`)

Compiler                                       | Elapsed   | Instructions   | Cycles        | IPC  | Branch Misses
----------------------------------------------|-----------|----------------|---------------|------|----------------
lvbfc (--no-strict --no-wrap --opt-level=1)   | 0.589 s   | 4.23B          | 2.64B         | 1.60 | 4.25 %
lvbfc (--no-strict --no-wrap --opt-level=0)   | 0.590 s   | 4.24B          | 2.64B         | 1.64 | 4.30 %
lvbfc (--no-strict --heap --opt-level=1)      | 1.021 s   | 11.61B         | 4.57B         | 2.54 | 1.98 %
lvbfc (--no-strict, default)                | 1.340 s   | 9.26B          | 5.36B         | 1.73 | 3.10 %
[bfjitc -O3 (C)](https://github.com/tsoding/bfjit)     | 1.586 s   | 5.54B          | 3.59B         | 1.54 | 4.00 %
[btc (LLVM)](https://github.com/Wilfred/bfc)                                            | 0.730 s   | 5.97B          | 3.27B         | 1.83 | 4.15 %
Python (brainfuck.py)                          | 1830.571 s| 25.19T         | 5.95T         | 4.24 | 0.08 %

## TODO

- [ ] Multi-file compilation support

>[!WARNING]
> --opt-level=1 can mess with really complex programs like utm.b or bitwidth.b, use --opt-level=0 for these, it still does heavy opts, but not as aggressively, every other program works just fine. (It'll fail with the cc/clang command, so you'll know, i'm working on it, but i'm not a magician)
