// Build with, e.g:
//   # cl.exe binary_to_compressed_c.cpp
//   # g++ binary_to_compressed_c.cpp
//   # clang++ binary_to_compressed_c.cpp
// You can also find a precompiled Windows binary in the binary/demo package available from https://github.com/ocornut/imgui

// Usage:
//   binary_to_compressed_c.exe [-base85] [-nocompress] [-nostatic] <inputfile> <symbolname>
// Usage example:
//   # binary_to_compressed_c.exe myfont.ttf MyFont > myfont.cpp
//   # binary_to_compressed_c.exe -base85 myfont.ttf MyFont > myfont.cpp