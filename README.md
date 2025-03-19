# Latimer

Statically-typed interpreted scripting language

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
