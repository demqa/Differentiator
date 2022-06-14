# Differentiator

You can you this program to calculate derivative of some expression

## Overview

Differentiator reads math expression using my Recursive Descent Parser into binary tree.
Then it builds differentiated expression, rearranging the tree. Lastly it simplifies expression
(p.e. 0 * anything -> 0), and then generates TeX file with final expression. Also there are steps,
that differentiator did to get this expression.

## Available Functions, Operators & Special Numbers

1. +, -, *, / - simple math operators
2. ^ - power
3. sin(x), cos(x) - trigonometric functions
5. e - Euler's number
4. ln(x) - natural logarithm
5. lg(x) - base 10 logarithm
6. pi - pi number
7. sh(x), ch(x) - hyperbolic functions

## Dependencies

1. Dot
2. PDFLaTeX

## Usage

You may write your math expression in expr1 file using several rules.
You cannot add spaces between characters and you should add `$` sign
at the end of expression.

```
mkdir -p Obj && make
./diff
```
