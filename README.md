# bfc

Brainfuck-to-C optimizing compiler written in C, using the [llv](https://github.com/lvzrr/llv) library.

## Features

- Converts Brainfuck code into """optimized""" C
- Optional optimizations for patterns like [-], ->+<, etc.
- Strict mode for loop sanity checks (honestly is too much but whatever)
- Output compiles to a native binary

## Build

make

## Usage

./bfc file.b [output] [options]

### Options

--no-strict    disable safety checks on loops  
--opt          enable optimizer  
--dmp-tok      dump token stream to stdout

## Requirements

- A C compiler (GCC or Clang)
- make
- Linux or POSIX system (or Windows with changes)

## Mandelbrot Benchmark (compiled with `bfc`)

| Metric               | Value              |
|----------------------|--------------------|
| **Time elapsed**     | 0.999 s            |
| **Instructions**     | 11.5 billion       |
| **Cycles**           | 4.21 billion       |
| **IPC**              | 2.73               |
| **Branch misses**    | 1.44%              |

## Tests

Includes some of the test shown here: https://brainfuck.org/tests.b
