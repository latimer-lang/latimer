# TODOS

### DOC
- [ ] doc/language grammar isn't very accurate. lk change to just examples?

### MAIN
- [x] read user input one line at a time (currently reading by spaces)
- [x] support taking in files
- [ ] wtf is going on w/ `1 < 3 : 4 ? 2`
- [ ] Modularize runtime values and token values separately, maybe put the values in the context of parser
- [ ] Allow forward declaration of functions and structs

### Lexical Analysis
- [x] build error handler
- [x] build lexer
- [x] change reserved keywords ("NIL" -> NULL, remove "VAR")
- [x] add support for binary operators (>>, <<, |, &, ^, ~)
- [x] add support for ternary expressions (?, :)
- [x] might have to specify integer literal value to be `int32_t`
- [ ] add support for walrus operators (assignment expressions)
- [x] remove PRINT Token after completion; on website as well
- [x] add support for `break;` and `continue;`
- [x] ignore /* */
- [x] add support for `double` types and remove `float` types. likewise, `int32_t` -> `int64_t`
- [ ] add support for lambda functions
- [ ] add function type annotations: `returnTy(argTy1, argTy2)`

### Parsing
- [x] create document for defining grammar for expressions
- [x] AST classes for expressions
- [ ] AST classes for statements, and other things
- [x] implement AST pretty printer for debugging
- [x] watch out when parsing `else if`

### Semantic Analysis
- [ ] BIG type system incoming...
- [ ] semantic error for `break` or `continue` not being in the loop
- [ ] error for shadowing native function names
- [ ] return statement cannot exist not inside functions

### Code Generation

### AstInterpreter
- [ ] implement short circuiting to logical operators
- [ ] more specific errors messages (ex: trying to add "a" + 'b' gives: "Unsupported operands for 'a' + 'b'". but ideally, should be "a" + 'b' to distinguish string and char)
- [x] possible move out type R to another class for more modularity
- [x] make default value of variable declarations nothing (std::monostate) instead of like nil, make sure it errors if the undeclared variable is being evaluated

### MISC
- [ ] pretty printer for statements
- [ ] allow repl to take in expressions (lk maybe not)
- [x] documentation website add operators