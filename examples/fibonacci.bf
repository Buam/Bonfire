// Use library "std"
(std)

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
  std::println(fib(0, 1, 1000))
  <- 0
}