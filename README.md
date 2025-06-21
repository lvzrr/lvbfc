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

./bfc file.b [output] [options]

### Options

--no-strict    disable safety checks on loops  
--opt          enable optimizer  
--dmp-tok      dump token stream to stdout  
--bound-30k    use static 30,000-cell buffer (legacy mode)

## Requirements

- A C compiler (GCC or Clang)
- make
- Linux or POSIX system (or Windows with changes)

## Mandelbrot Benchmark

| Mode              | Time Elapsed | Instructions     | Cycles         | IPC  | Branch Misses |
|-------------------|--------------|------------------|----------------|------|----------------|
| Bounded 30K       | 0.623 s      | 4.31 billion     | 2.63 billion   | 1.64 | 4.34%          |
| Growable  | 0.999 s      | 7.81 billion     | 4.00 billion   | 1.95 | 1.02%          |

Note: The bounded mode is faster but uses a static 30,000-cell array. The growable version is safer in terms of memory overflows but slower.

Note: --opt flag introduces nontrivial macro expansion logic to the C compiler, take that in mind

## Tests

Includes some of the tests shown here: https://brainfuck.org/tests.b
