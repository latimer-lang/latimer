# Introduction
This doc contains notes used to create Latimer's recursive-descent parser.

## Latimer Operator Precedence
The following table lists the precedence and associativity of Latimer operators. Operators are listed top to bottom, in ascending precedence. a, b and c are operands.
| **Name**   | **Operator**                      | **Description**                                  | **Associativity** |
|------------|-----------------------------------|--------------------------------------------------|-------------------|
| ternary    | `a ? b : c`                       | Ternary conditional                              | right             |
| logical    | `a \|\| b` `a && b`               | Logical OR and AND                               | left              |
| bitwise    | `a \| b` `a & b` `a ^ b`          | Bitwise OR, AND, and XOR                         | left              |
| equality   | `a == b` `a != b`                 | Equality operators = and != respectively         | left              |
| comparison | `a > b` `a >= b` `a < b` `a <= b` | Relation operators >, >=, <, and <= respectively | left              |
| bitshift   | `a >> b` `a << b`                 | Bitwise right shift and left shift               | left              |
| term       | `a - b` `a + b`                   | Subtraction and addition                         | left              |
| factor     | `a / b` `a * b` `a % b`           | Division, multiplication, and remainder          | left              |
| unary      | `!a` `~a` `-a`                    | Logical NOT, bitwise NOT, and unary minus        | right             |

## Expressions
This section consists of Context-Free Grammar used by Latimer's parser for expressions.

```
expression → ternary
ternary → logical ( "?" | logical ":" ternary )?
logical → bitwise ( ( "||" | "&&" ) logical )*
bitwise → equality ( ( "|" | "&" | "^" ) equality )*
equality → comparison ( ( "==" | "!=" ) comparison )*
comparison → bitshift ( ( ">" | ">=" | "<" | "<=" ) bitshift )*
bitshift → term ( ( ">>" | "<<" ) term )*
term → factor ( ( "-" | "+" ) factor )*
factor → unary ( ( "/" | "*" | "%" ) unary )*
unary → ( "!" | "~" | "-" ) unary | primary
primary → NUMBER
        | STRING
        | CHARACTER
        | "true"
        | "false"
        | "null"
        | "(" expression ")"
```