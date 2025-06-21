# bfc

Brainfuck-to-C optimizing compiler written in C, using the [llv](https://github.com/lvzrr/llv) library.

## Features

- Converts Brainfuck code into """optimized""" C
- Optional optimizations for patterns like [-], ->+<, etc.
- Strict mode for loop sanity checks (honestly is too much but whatever)
- Output compiles to a native binary
- Optional legacy mode using a 30,000-cell static array (faster but less safe)

## Build

make

## Usage

```

bfc - Brainfuck compiler
---------------------------------------------------
Usage:
  ./bfc <input.b> [output] [options]

Options:
  --no-strict       Disable safety checks for loops
                    Ignores checks for potentially infinite loops

  --bound-30k       Use legacy 30,000-cell memory model
                    Faster, dumber, and prone to all the fun bugs
                    Good for benchmarks, bad for safety

  --opt             Enable token stream optimization
                    Recognizes simple idioms like [-] as clear cell
                    Both the default and this version have dynamic
                    memory management and bound checks for each move.

  --dmp-tok         Print parsed token stream
                    For debugging

  --help            Print this very help message

Output:
  Produces a compiled C file and builds it to a native binary.
  Uses GCC or Clang.
```

## Requirements

- A C compiler (GCC or Clang)
- make

## Mandelbrot Benchmark

| Mode              | Time Elapsed | Instructions     | Cycles         | IPC  | Branch Misses |
|-------------------|--------------|------------------|----------------|------|----------------|
| Bounded 30K       | 0.623 s      | 4.31 billion     | 2.63 billion   | 1.64 | 4.34%          |
| Growable  | 0.999 s      | 7.81 billion     | 4.00 billion   | 1.95 | 1.02%          |

>[!WARNING]
> The bounded mode is faster but uses a static 30,000-cell array. The growable version is safer in terms of memory overflows but slower.

> [!WARNING]  
> --opt flag introduces nontrivial macro expansion logic to the C compiler, take that in mind

## Tests

Includes some of the tests shown here: https://brainfuck.org/tests.b for compliance

Also includes some programs from here: https://brainfuck.org/ (they are really cool check them out)

## Running programs

As of right now, all of these are running smoothly (these are in tests/) + a couple more:

```
arr2.b
arrs.b
dbf2c.b
e.b
helloworld.b
io.b
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

