# Code conventions

This is a modified version of kernel coding style. This coding style is applied in Stateplex library project. If you want to contribute to this project you need to follow this guide.

## 1. Indentation

* Tabs are 8 characters and make reading easier, therefore we use tabs indentation, which also gives the limit for tending to use too deep nestings in the code.
* Avoid putting multiple statements into a single line.
* Avoid putting multiple assignments into single line.
* Keep it simple (e.g. one issue per one function, no tricky code, no hard coding).
* No white spaces in the end of lines.


## 2. Breaking long lines and strings

* 80 characters is the soft limit for the length of line. If over 80 characters and you find a good place for next line break you probably should do a line break. Do not compromise readability or do not shorten words.
* Descendants are shorter than parent lines, indentation 2 tabs(16 spaces).
* Previous applies also to function headers with long argument lists.
* Prefer descriptive words.
* Don’t brake printable, user visible strings shorter, since it brakes the availability to grep.


## 3. Placing Braces and Spaces

### 3.1 Braces

#### 3.1.1 Non-function statement blocks

* Opening brace last in the line.
* Closing brace in the beginning of the line.

### 3.2 Functions

* Opening and closing braces in the beginning of the line.

* Closing brace is empty of its own except when there is a continuation of the same statement.
* Use braces always.

### 3.3 Spaces

* Use space after keyword except if they look more like a function (like typeof, sizeof etc.).
* Surround ‘: ‘ with spaces e.g. in case of class inheritance.
* Put pointer adjacent to function name or data name, not adjacent to type name.
* No trailing whitespaces in the end of lines.
* Surround most binary and ternary operators with one space on both sides.
* Use unary operators without a space.
* Use unary prefix- and postfix operators without a space.
* No space with unary structure operators, like ‘.’ and ‘->’.


## 4. Naming

* Use descriptive names for global variables and functions.
* Local variable names should be short and to the point.
* In classes: write member variables with a leading ‘m’.
* Write static variables with a leading ‘s’.


## 5. Typedefs

* Don’t use typedef for pointers and structs that has elements, that can be accessed directly.


## 6. Functions

* Functions should be short and contain only one task.
* Functions should be maximum one or two screenfuls long.
* Use maximum 5 -10 local variables per function.
* Separate functions with a blank line.


## 7. Centralized exiting of functions

* Albeit deprecated, unconditional **goto** is acceptable in cases when:  
  * Unconditional statements are easier to understand and follow.
  * Reduced nesting.
  * Errors by updating exit points are prevented.
  * Optimizing redundant code.


## 8. Commenting

* See Doxygen README file for instructions how to document the code properly. IF NECESSARY USE THE FOLLOWING RULES FOR COMMENTING:
  * Do not over-comment, use concise comments.
  * Try not to use normal way of commenting, prefer doxygen documents. Leave licence comments as normal comments etc.
  * Write WHAT your code does or WHY it is there, not HOW it does it.
  * Use Linux style comments: /*...*/.
  * It’s important to comment data whether it is basic type or derived type.


## 9. Macros, Enums

* Names of macros defining constants should be capitalized.
* Labels in enums should be capitalized.
* Capitalized macro names are preferred but names of macros resembling functions can be written in lowercase letters.


## 10. The inline disease

* Use inline functions if it is necessary for performance reasons.
* Do not use otherwise.


## 11. Function return values and names

* If the name of the function is an action or an imperative command, the function should return an error code integer (-Exxx = failure, 0 = success).
* If the name of the function is a predicative, the function should return a ‘succeeded’ boolean (0 = failure, non-zero = success).




