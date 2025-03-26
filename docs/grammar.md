# Expression Grammar
This section consists of Context-Free Grammar for all types of expressions supported by Latimer.
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
        | FLOAT
        | STRING
        | CHAR
        | "true"
        | "false"
        | "NULL"
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
