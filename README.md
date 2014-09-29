OpenCL Memory Pattern Analyser
==============================

OpenCL Memory Pattern Analyser (*OMPA*) does a static code analysis about the
memory pattern of your OpenCL code.

This code is the result of my bachelor thesis at KIT. The bachelor thesis itself
and the presentation are in German.


Development
-----------

OMPA is written in C++ and make use of LLVM/Clang.


Build
-----

* **make**: Build the library
* **make check**: Build the library with DEBUG flag and runs a unit test
* **make release**: Runs make clean and build the library without the DEBUG flag
* **make clean**: Remove all compiled and temporary files
