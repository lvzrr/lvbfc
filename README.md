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
btc                                            | 0.730 s | 5.97B          | 3.27B         | 1.83 | 4.15 %

## Tests

Compliant with https://brainfuck.org/tests.b (included under tests/compliance) and includes various example programs.

## TODO

- [ ] LLVM IR emitter & LLVM backend
- [ ] Multi-file compilation support
- [ ] Smart syscall optimizer
- [ ] Detection of weird misuse of shellcode mode
