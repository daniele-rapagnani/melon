func testNumber |i| => {
    io.print(number.fromString(i));
}

testNumber("5e10");
testNumber("5e-5");
testNumber("5.456e3");
testNumber("5asdasd");

io.print(10 * 15);
io.print(10 * 15.0);
io.print(10.0 * 15);