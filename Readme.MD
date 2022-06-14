# Differentiator

You can you this program to calculate derivative of some expression

## Overview

Differentiator reads math expression using my Recursive Descent Parser into binary tree.
Then it builds differentiated expression, rearranging the tree. Lastly it simplifies expression
(p.e. 0 * anything -> 0), and then generates TeX file with final expression. Also there are steps,
that differentiator did to get this expression.

## Available Functions, Operators & Special Numbers

1. `+`, `-`, `*`, `/` - simple math operators
2. `^` - power
3. `sin(x)`, `cos(x)` - trigonometric functions
4. `e` - Euler's number
5. `ln(x)` - natural logarithm
6. `lg(x)` - base 10 logarithm
7. `pi` - pi number
8. `sh(x)`, `ch(x)` - hyperbolic functions

## Dependencies

1. Dot
2. PDFLaTeX

## Usage

You may write your math expression in expr1 file using several rules:
1. Variables can be only small Latin letters.
2. Numbers can be float-point.
3. Differentiating is only by `x` variable. You can change it in code.
4. You cannot add spaces between characters.
5. You have to add `$` sign at the end of expression.
6. You can skip brackets when you use operator with the same priority.
(p.e. you can write 1+x+y-5 or 4*x*ln(x))
7. You can skip backets when arguement of some math function is the only.
(p.e. you can write lnx instead of ln(x), 2^2 instead of 2^(2), sinx^2 -> (sin(x))^2)

#### Example

```
sinx^2+lnx+ln((x*y+x-y)^2)$
```

```
mkdir -p Obj && make
./diff
```
