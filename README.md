# Latimer

A general-purpose statically-typed interpreted language designed for efficiency, safety, and developer productivity.

## Compiling the Compiler

```bash
# assuming you are in the root directory
cmake -S . -B build
# -S . -> source directory is the current directory (aka the root)
# -B build -> build output goes into the build directory (which you should have created)
```

## Code Style

Run the clang-format on all files
```bash
clang-format -i **/*.cpp **/*.hpp
```

## TODOS

### DOC
- [ ] doc/language grammar isn't very accurate. lk change to just examples?

### MAIN
- [x] read user input one line at a time (currently reading by spaces)
- [x] support taking in files
- [ ] wtf is going on w/ `1 < 3 : 4 ? 2`

### Lexical Analysis
- [x] build error handler
- [x] build lexer
- [x] change reserved keywords ("NIL" -> NULL, remove "VAR")
- [x] add support for binary operators (>>, <<, |, &, ^, ~)
- [x] add support for ternary expressions (?, :)
- [x] might have to specify integer literal value to be `int32_t`
- [ ] add support for walrus operators (assignment expressions)
- [ ] remove PRINT Token after completionß
- [ ] support both semicolon and newline

### Parsing
- [x] create document for defining grammar for expressions
- [x] AST classes for expressions
- [ ] AST classes for statements, and other things
- [x] implement AST pretty printer for debugging
- [ ] watch out when parsing `else if`

### Semantic Analysis

### Code Generation

### AstInterpreter
- [ ] implement short circuiting to logical operators
- [ ] more specific errors messages (ex: trying to add "a" + 'b' gives: "Unsupported operands for 'a' + 'b'". but ideally, should be "a" + 'b' to distinguish string and char)
- [x] possible move out type R to another class for more modularity

### MISC
- [ ] pretty printer for statements