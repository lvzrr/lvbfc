# lvbfc

Brainfuck-to-C optimizing compiler written in C, using the [llv](https://github.com/lvzrr/llv) library.

## Features

- Converts Brainfuck code into """optimized""" C
- Optional optimizations for patterns like [-], ->+<, etc.
- Strict mode for loop sanity checks (honestly is too much but whatever)
- Output compiles to a native binary
- Wrapping buffer, no memory unbound actions

## Build

make

## Usage

```

lvbfc - Brainfuck compiler
---------------------------------------------------
Usage:
  ./lvbfc <input.b> [output] [options]

Options:
  --no-strict       Disable safety checks for loops
                    Ignores checks for potentially infinite loops

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

| Compiler             | Time Elapsed | Instructions     | Cycles         | IPC  | Branch Misses |
|-------------------|--------------|------------------|----------------|------|----------------|
| lvbfc       | 0.623 s      | 4.31 billion     | 2.63 billion   | 1.64 | 4.34%          |


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

## TODO:

- [x] add legacy layout
- [ ] add LLVM IR emitter + target
- [ ] allow multi-file-compilation
