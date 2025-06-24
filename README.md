# lvbfc (lv's Brainfuck compiler & superset)

Brainfuck to C transpiler + native binary compiler written in C, built using the [llv](https://github.com/lvzrr/llv) library.

Supports standard Brainfuck, as well as a rich extended superset that includes features like shellcode execution, raw pointer manipulation, and memory intrinsics transpiled to fast, optimized C code.

For more details on extensions, usage, implementation, and examples, see [INFO.md](./INFO.md).

## Features

| Feature                   | Description |
|---------------------------|-------------|
| Optimized Transpilation   | Converts Brainfuck into fast, readable C code |
| Operation Folding         | Combines repetitive operations (e.g., `++++` → `*buf += 4`) |
| Strict Loop Analysis      | Validates loop pairing and nesting in strict mode |
| Dead Code Elimination     | Removes ineffective operations and empty loops |
| Native Binary Output      | Compiles to native ELF via `gcc`/`clang` (fallback safe) |
| Wraparound Control        | Circular buffer logic toggle via `--no-wrap` |
| Heap Mode (`--heap`)      | Dynamic tape growth with realloc and guards |
| Shellcode Mode (`--x`)    | Enables `;` to write and exec syscalls from buffer |
| Intrinsics (`$`)          | Powerful shortcuts: memset, memcpy, math ops, all tape-driven |
| Canary Debugging          | `?` adds comments; `??` dumps buffer state at runtime |
| Extended Pointer Ops      | `&`, `=` allow storing/following raw pointers on tape |
| Debug Stream Dump         | `--dmp-tok` prints token list pre-emit |
| Minimal Dependencies      | Pure C, builds with `make` and standard compiler |
| Backward compatible       | Superset functionallity is 100% optional, default idiomatic Brainfuck is fully supported |

## Build

```sh
make
```

>[!NOTE]
>running `make` will autofetch llv as it has its own repo

## Usage
```
lvbfc - Brainfuck compiler
---------------------------------------------------
Usage:
  ./lvbfc <input.b> [output] [options]

Arguments:
  <input.b>             Brainfuck source file (required)
  [output]              Output binary name (optional)
                        Default: bfout

Options:
  --no-strict           Disable compile-time loop validation
  --no-wrap             Disable tape wraparound (exits on overflow)
  --heap                Use dynamic memory instead of fixed-size tape
  --turbo               Skip all bounds checks (manual pointer safety)
  --x                   Enable syscall emit mode using ';' bytes
  --allow-canary        Enable '?' and '??' debug markers + memory dump
  --allow-intrinsics    Enable '$' for memset, math, and other builtins
  --stacksize=N         Initial tape size (default: 65536 bytes)
  --opt-level=N         Optimizer passes (0 = off, >0 = enabled)
  --dmp-tok             Dump token stream before compilation (debug)
  --help                Show this help message and exit

Output:
  Produces .c and native binary using GCC or Clang (fallback supported)

Examples:
  ./lvbfc hello.b hello           # compile to ./hello
  ./lvbfc code.b --heap           # dynamic memory mode
  ./lvbfc syscall.b --x           # emit + exec syscall shellcode
  ./lvbfc file.b --stacksize=0    # compile error: invalid size

Suggestions:
  - Fastest:             --no-wrap --opt-level=1 --turbo
  - Mid‑range:           --heap
  - Shellcode mode:      --x with ';;;;;;' to emit 6‑byte syscall
  - Safe default:        no flags
```
