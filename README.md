# Bonfire
![Progress](https://img.shields.io/badge/Progress-20%25-orange "Progress: 20%")
![License](https://img.shields.io/badge/License-MIT-green "License: MIT")
[![CodeFactor](https://www.codefactor.io/repository/github/buam/bonfire/badge)](https://www.codefactor.io/repository/github/buam/bonfire)
![Status](https://img.shields.io/badge/Status-Works%20on%20my%20machine-yellowgreen "Works on my machine")
[![HitCount](http://hits.dwyl.com/Buam/Bonfire.svg)](http://hits.dwyl.com/Buam/Bonfire)

Primitive compiler for a language I called Bonfire.  
Compiles to Intel-Syntax Assembly for the GNU Assembler (part of [GCC](https://gcc.gnu.org))

Current example (An example that shows the most recent features):
```rust
// Main function (returns 32-bit integer)
main() -> i32 {
  // Declare a variable named 'x' and initialise it with the constant 10
  x: i32 = 10
  // Return the value of x
  <- x
}
```


### Language Features
Most of these features are not yet implemented, there is a list of already implemented features below.
#### Code Blocks
The language is based on Code Blocks. A code block looks like this:
```rust
{
  // Code block that returns void
}
```
It can return something:
```rust
-> i32 {
  // Code block that returns i32
  <- 3
}
```
Function bodies are expressions. A Code Block is an expression, so if you want to use functions the traditional way, the expression of your function will be a code block.
```rust
main() -> i32 { // Function bodies are code blocks
  ?(true) {
    // This code block returns void
    // If we return something anyways, it will return for the parent code block (the function)
    <- 3
  }
}
```
#### Control Structures
```rust
// If
?(condition) expression : expression
// Example
?(true) {
  // Do something
} : {
  // Do something else
}
```

```rust
// Loop (while)
|(condition) expression
// Example
|(true) {
  // An infinite loop
}
```
Since all these structures are expressions, you can use them like this, for example:
```rust
x: i32 = ?(true) 2 : 3
// Same thing with code blocks
y: i32 = ?(true) { <- 2 } : { <- 3 }
```

This was just to describe the language syntax. Most of these features are not yet implemented.
### Feature List
* [x] - Implemented
* [ ] - Not yet implemented

------------------

* [ ] Basic structures
  * [ ] If
  * [ ] Loop
  * [x] Code block
* [ ] Variables
  * [x] Declaration and initialisation
  * [x] Get Value
  * [ ] Value assignment
* [ ] Functions
  * [ ] Parameters
    * [ ] By value
    * [ ] By reference
  * [ ] Function call
* [ ] Libraries
  * [ ] Metadata
  * [ ] Correct linking

Licensed under the [MIT License](https://github.com/Buam/Bonfire/blob/master/LICENSE.txt)
