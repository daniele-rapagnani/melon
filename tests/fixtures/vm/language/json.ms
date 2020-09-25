
func testJsonStr |str| => {
    let result = json.parse(str);
    io.print(json.stringify(result));
}

func testJson |file| => {
    let result = json.parseFile(file);
    io.print(json.stringify(result));
}

func testParse |file| => {
    let result = json.parseFile(file);
}

testJson("fixtures/vm/language/json_test_files/value.json");
testJson("fixtures/vm/language/json_test_files/array.json");
testParse("fixtures/vm/language/json_test_files/object.json");
testParse("fixtures/vm/language/json_test_files/test1.json");