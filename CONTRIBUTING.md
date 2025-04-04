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
