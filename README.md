# Pointer-Compare
Static analyzer that reports all comparisons between two pointers of a specified type.

alexandermartin006@gmail.com

# Building (Windows)

Run ```build.bat```

All necessary libraries and includes are already in this repository. I got the compiled clang libraries from here: https://github.com/ziglang/zig/wiki/Building-Zig-on-Windows

To build on Linux make sure the libclang library and header files (the clang-c folder) are in your path.

# Usage

```analyze [path to a c/c++ source file] [TypeName] ```

# Output

Check out test1.cpp in the tests folder.

Running ```analyze ../tests/test1.cpp Atom``` from the build folder gives the following output:

```
Comparing Atom pointers in ../tests/test1.cpp on line 12
Comparing Atom pointers in ../tests/test1.cpp on line 14
Comparing Atom pointers in ../tests/test1.cpp on line 16
Comparing Atom pointers in ../tests/test1.cpp on line 18
Comparing Atom pointers in ../tests/test1.cpp on line 21
Comparing Atom pointers in ../tests/test1.cpp on line 25
Comparing Atom pointers in ../tests/test1.cpp on line 27
Comparing Atom pointers in ../tests/test1.cpp on line 31
Comparing Atom pointers in ../tests/test1.cpp on line 34
Comparing Atom pointers in ../tests/test1.cpp on line 36
Comparing Atom pointers in ../tests/test1.cpp on line 39
```