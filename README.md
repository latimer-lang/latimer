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

### Lexical Analysis
- [x] build error handler
- [x] build lexer
- [x] change reserved keywords ("NIL" -> NULL, remove "VAR")
- [ ] add support for binary operators (>>, <<, |, &, ^)

### Parsing
- [x] create document for defining grammar for expressions
- [ ] AST classes
- [ ] implement AST pretty printer for debugging
- [ ] watch out when parsing `else if`

### Semantic Analysis

### Code Generation
