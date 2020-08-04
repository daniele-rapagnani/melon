let c = compiler.compile("io.print(5);");

let module = { path = null };

c(module);
c(module);
c = compiler.compile("return 10;");
io.print(c(module));

c = compiler.compile("testtest", |message, file, line, col| => {
    io.print("Error at " .. string.toString(file) .. " (" .. string.toString(line) .. ":" .. string.toString(col) .. ") - " .. message);
});

c = compiler.compile("if(", |message, file, line, col| => {
    io.print("Error at " .. string.toString(file) .. " (" .. string.toString(line) .. ":" .. string.toString(col) .. ") - " .. message);
});

c = compiler.compile("asdasd2", |message, file, line, col| => {
    io.print("Error at " .. string.toString(file) .. " (" .. string.toString(line) .. ":" .. string.toString(col) .. ") - " .. message);
});

c = compiler.compile("{}");
c(module);

c = compiler.compile("return |num| => { return num^2; };");
io.print(c(module)(5));

c = compiler.compile("let a = 5; return |num| => { return a * num; };");
io.print(c(module)(10));

c = compiler.compileFile("wrong_file.ms");
io.print(c);

c = compiler.compileFile("fixtures/vm/language/compiler_test.src");
io.print(c);
c(module).myStuff();

c = compiler.compileFile("fixtures/vm/language/compiler_test_fail.src", |message, file, line, col| => {
    io.print("Error at " .. string.toString(file) .. " (" .. string.toString(line) .. ":" .. string.toString(col) .. ") - " .. message);
});