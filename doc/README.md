General
=======

Stateplex uses Doxygen for source code documentation.

Doxygen is a widely used tool for generating documentation from annotated C++ source files. It also supports many other programming languages. With Doxygen you can create online documentation in HTML format and you can extract the code structure from undocumented source files, which is good for visualizing the relations between various elements. The possible diagrams are include dependency graph, inheritance diagrams and collaboration diagrams and they are generated automatically.

Making documentation requires that you have Doxygen installed on your system. You can make documentation by writing ‘make docs’ after running ‘./configure’. That will create html output of the documentation to the doc folder. By opening index.html in your web browser, you can navigate through the documentation in html form.

Writing comments
================

Style
-----

Comments are in JavaDoc style. Comment blocks for functions should be always before implementation, so usually in source (.cpp) files but in some cases in header (.h) files.

Files
-----

Every source and header file should have a comment block in the beginning stating at least the file name, author and short description of the purpose of the file

Functions
---------

Every function should have a comment block before its implementation. It should contain at least a short and concise description of what the function does, descriptions of all the parameters it gets and the return value.

Examples
========

File header comment
-------------------

```
/**
* Doxygen testing class, this is brief description
*
* @file doxygen.h
* @author Team Stateplex
* @see doxygen.cpp
*
*/
```

File header comment should be in each source and header file.

Function comments
-----------------

```
/**
* Function that returns the bigger number of the parameters given.
*
* @param first, integer value
* @param second, integer value
* @return max, integer value
*/
```

Each function should be commented in this way. Remember to put the comment always before the implementation (either in the .cpp or the .h file) and NOT before the prototype/definition of the function. Of course the param and return might be missing if the function does not take any parameters or returns void.

Main page comments (for doxygen output, in mainpage.dox)
--------------------------------------------------------

```
/**
* @mainpage Mainpage header
* @section intro_sec introduction topic
* Introduction text
* @section install_sec Installation topic
* @subsection Step 1: Opening the box
* etc...
*/
```

The project must only have one mainpage.dox file and it should have general information about the project and its targeted purpose.
