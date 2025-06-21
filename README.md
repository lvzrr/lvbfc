# lvbfc

Brainfuck-to-C optimizing compiler written in C, using the [llv](https://github.com/lvzrr/llv) library.

## Features

- Converts Brainfuck code into """optimized""" C
- Optional optimizations for patterns like [-], ->+<, etc.
- Strict mode for loop sanity checks (honestly is too much but whatever)
- Operation cancellation/balancing
- Code sanitization, dead code removal
- Infinite loop removal '[]'
- Output compiles to a native binary
- Wrapping buffer, no memory unbound actions (optional)

## Build

make

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
  --no-strict       Disable safety checks for loops
                    Ignores checks for potentially infinite loops

  --no-wrap         Disable cell wraparound (executes faster)
                    Allows 8-bit overflow behavior to be undefined

  --use-heap        Use heap allocation for memory (dynamic growth)
                    Enables GROW_BUF and dynamic pointer range

  --stacksize=N     Set initial memory size in bytes
                    Default: 65536 (64 KiB)

  --dmp-tok         Print parsed token stream
                    Useful for debugging and optimization testing

  --help            Show this help message and exit

Output:
  Produces a compiled C file and builds it into a native binary.
  Uses GCC or Clang automatically (fallback if one fails).
  Output binary defaults to './bfout' unless specified.

Examples:
  ./lvbfc hello.b hello         # Compile hello.b to ./hello
  ./lvbfc code.b --use-heap     # Use heap-allocated memory
  ./lvbfc file.b --stacksize=0  # Will error out (invalid stacksize)

Suggestions:
  - Fastest:   --no-wrap        (you control stacksize manually)
  - Safer:     --use-heap       (grows memory on demand)
  - Slowest:   Default wrapping (safe but performs worst)
```

## Requirements

- A C compiler (GCC or Clang)
- make

## Mandelbrot Benchmark
| Compiler             | Time Elapsed | Instructions     | Cycles         | IPC  | Branch Misses |
|----------------------|--------------|------------------|----------------|------|----------------|
| lvbfc (--no-wrap)    | 0.623 s      | 4.31 billion     | 2.63 billion   | 1.64 | 4.34%          |
| lvbfc (--use-heap)   | 0.985 s      | 4.31 billion     | 2.63 billion   | 1.64 | 4.36%          |
| bfjitc               | 1.586 s      | 5.54 billion     | 3.59 billion   | 1.54 | 4.00%          |
| lvbfc (--no-wrap 30k)| 1.631 s      | 11.50 billion    | 4.18 billion   | 2.75 | 1.45%          |
| lvbfc (default wrap) | 3.014 s      | 14.08 billion    | 10.87 billion  | 1.29 | 2.83%          |


## Tests

Compliant with https://brainfuck.org/tests.b (included under tests/compliance)

Also includes some programs from here: https://brainfuck.org/ (they are really cool check them out)

## Running programs

As of right now, all of these are running smoothly (these are in tests/) + a couple more:

```
dbf2c.b
e.b
helloworld.b
jabh.b
life2.b
mandelbrot.b
numwrap.b
pgq.b
pwo2.b
quine.b
rbg.b
stri.b
truemorse.b
ttt.b
unmatched.b
utm.b
```

## TODO:

- [ ] add LLVM IR emitter + target
- [ ] allow multi-file-compilation
