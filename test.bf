main() -> i32 {
	x: i32 = 2
	?(x == 2) {
		<- 3
	}
	<- x
}