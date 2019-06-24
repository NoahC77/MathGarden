# Math Garden

	This project's goal is to learn about writting programs that use multiple cores, improve how I write these programs, and have fun combining programming with various things from math.This project uses a number of libraries and source code from other code sources, the first being [PASL](https://github.com/deepsea-inria/pasl). PASL stands for Parallel Algorithm Scheduling Library and interestingly is pernounced "PASOLE". This library along with it's bash scripts is used to bundle together various dependencies to allow the project to fork with c++ lambda expression among many other things it is used alot in this project. Another library currently used is the computer algebra library [Symbolicc++](symboliccpp.sourceforge.net), however this may soon change to meet the goal of user defined mathematical functions for the simpson estimation. Lastly the library [exprtk](www.partow.net/programming/exprtk/index.html) is used aswell for things like evaluating mathmatical functions at given points. exprtk stands for Mathematical Expression Parsing And Evaluation library.

##Getting Started
	The code used from the previously mentioned repositories and the source code has the following dependencies.
- [gcc](https://gcc.gnu.org)
- [php](https://www.php.net)
- [ocaml](www.ocaml.org)
- [Symbolicc++](symboliccpp.sourceforge.net)
- [exprtk] (www.partow.net/programming/exprtk/index.html)
- c++11
- Unix

To generate the .opt file
```
make simpson.opt -j
```

To run the .opt with a number of cores.
```
./simpson.opt -n 45 -proc 1
```
In this example only one core is used, if you want to use 'x' number of cores do
```
./simpson.opt -n 45 -proc x
```

##TODO
1. Generate .d files for all headers used in simpson estimation.
2. Clean up simpson estimation program and improve efficiency.
3. Allow user defined mathematical functions for the simpson estimation program.
4. Thoughtfully design and program the vaneck sequence program to write the vaneck sequence of user defined length to a text file.

