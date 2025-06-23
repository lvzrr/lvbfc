# lvbfc (lv's Brainfuck compiler)

Brainfuck to C transpiler + compiler written in C, using the [llv](https://github.com/lvzrr/llv) library.

## Features
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

## Build

```sh
make
```

>[!NOTE]
>running `make` will autofetch llv as it has it's own repo

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
  --allow-canary    Enable `?` and `??` instructions (buffer inspection)
  --allow-intrinsics Enable special `$` operations (memset, math, etc.)
  --stacksize=N     Initial memory size in bytes (default: 65536)
  --opt-level=N     Optimization passes (default: 0), currently trivial
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
  - Mid‑range:           --heap
  - Shellcode mode:      --x with `;;;;;;` to exec 6‑byte syscall
  - Safe default:        no flags
```

For more information, refer to [INFO.md](./INFO.md).
