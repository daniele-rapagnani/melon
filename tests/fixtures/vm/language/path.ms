func testPath |pathIn| => {
    let funcs = ["dirname", "basename"];

    for (let f in funcs) {
        io.print(path[f](pathIn));
    }
}

testPath("");
testPath("test");
testPath("test.ext");
testPath("/test.ext");
testPath("/");
testPath("..");
testPath("very/long/relative/path/to/file.ext");
testPath(this.path);
