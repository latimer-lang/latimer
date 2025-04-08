# Introduction
This doc contains notes used to create Latimer's lexer.

# Expressions
This section consists of Context-Free Grammar for all types of expressions in Latimer.
```
expression → literal
            | group
            | unary
            | binary
            | ternary
```

## Literals
```
literal → NUMBER
        | DOUBLE
        | STRING
        | CHAR
        | "true"
        | "false"
        | "null"
```

## Group Expressions
```
group → "(" expression ")"
```

## Unary Expressions
```
unary → ( "-" | "!" | "~" ) expression
```

## Binary Expressions
```
binary → expression operator expression

operator → "+"
        | "-"
        | "*"
        | "/"
        | "%"
        | "&"
        | "|"
        | "^"
        | "<<"
        | ">>"
        | "<"
        | "<="
        | ">"
        | ">="
        | "=="
        | "!="
        | "&&"
        | "||"
```

## Ternary Expressions
```
ternary → expression "?" expression ":" expression
```
