
func testInt |i| => {
    io.print(integer.fromString(i) ?? 0);
}

testInt("asdasd");
testInt("1");
testInt("1000");
testInt("15");
testInt("0");
testInt("+125");
testInt("-123124");