# Bonfire
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
Future examples, to showcase the syntax (not the currently available features):

**Fibonacci Sequence:**
```rust
// Use library "console"
(console)

// Finds the biggest number smaller than max in the fibonacci sequence
fib(a: i32, b: i32, max: i32) {
  // Declare x, y and z
  x: i32 = a
  y: i32 = b
  z: i32 = 0
  
  // While loop
  | {
    z = x + y
    ?(z > max) {
      // Return y
      <- y
    }
    x = y
    y = z
  }
}

main() -> i32 {
  // Use function "println" from library "std" to print one line on the screen
  console::println(fib(0, 1, 1000))
  <- 0
}
```
