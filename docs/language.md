# Latimer Language Reference

## Datatypes
Latimer supports 5 datatypes: `bool`, `int`, `float`, `char`, `string`.

Datatype syntax:
```
type → "bool" 
        | "int"
        | "float"
        | "char"
        | "string"
```

Datatype value syntax:
```
value → bool
        | int
        | float
        | char
        | string
bool → "true" 
        | "false"
int → NUMBER
float → NUMBER "." NUMBER
char → "'" CHARACTER "'"
string → """ STRING """
```

Example:
```
bool a = true
int b = 10
float c = 2.0
float c = 2. // this is not a valid syntax
float c = .2 // neither is this
char d = 'a'
string e = "hello world!"
```

## Arithmetic
Latimer supports the following arithmetic expressions.

Binary Arithmetic Syntax:
```
binary_arithmetic → value arithmetic_operator value
arithmetic_operator → "+"   // addition
                    | "-"   // subtraction
                    | "*"   // multiplication
                    | "/"   // division
                    | "%"   // modular arithmetic
                    | "&"   // binary AND
                    | "|"   // binary OR
                    | "^"   // binary XOR
                    | "<<"  // binary shift-left
                    | ">>"  // binary shift-right
```

Unary Arithmetic Syntax:
```
unary_arithmetic → "-" value // negation
```

## Logical
Latimer supports the following logical expressions.

Binary Logical Syntax:
```
binary_logical → value logical_operator value
logical_operator → "<"  // less than
                | "<="  // less than or equal to
                | ">"   // greater than
                | ">="  // greater than or equal to
                | "=="  // equal to
                | "!="  // not equal to
                | "&&"  // logical AND
                | "||"  // logical OR
```

Unary Logical Syntax:
```
unary_logical → "!" value // negation
```

Ternary Logical Syntax:
```
ternary_logical → value "?" value ":" value // if _ then _ else _
```

## Statements

Variables:
```
type identifer = value
```

Control Flow:
```
if (condition) {
    ...
} else if {
    ...
} else {
    ...
}

while (condition) {
    ...
}

for (__;__;__) {
    ...
}
```

Function:
```
return_type func_name(param1, param2) {
    ...
}
```

Structs:
```
struct structName : parent {
    int x

    structName(params) {
        parent1(...)
        this.x = 1
    }

    int getX() {
        return x
    }
}

structName s = structName(...)
print(s.getX())
```

## Standard Library
```
print()
clock()
```
