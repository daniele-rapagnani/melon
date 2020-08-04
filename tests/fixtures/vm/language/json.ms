
func testJson |file| => {
    let result = json.parse(file);
    io.print(json.stringify(result));
}

func testParse |file| => {
    let result = json.parse(file);
}

testJson("fixtures/vm/language/json_test_files/value.json");
testJson("fixtures/vm/language/json_test_files/array.json");
testParse("fixtures/vm/language/json_test_files/object.json");
testParse("fixtures/vm/language/json_test_files/test1.json");