
func test => {
    io.print("Void function");
}

func test2 |x| => {
    io.print("First arg: " .. string.toString(x));
}

func test3 |x, y| => {
    io.print("First arg: " .. string.toString(x));
    io.print("Second arg: " .. string.toString(y));
    return x .. y;
}

function.call(test, []);
function.call(test2, ["test"]);
function.call(test2, []);
function.call(test2, ["test", "test2"]);
io.print(function.call(test3, ["test", "test2"]));

io.print(function.getName(test3));
io.print(function.getName(test));
io.print(function.getName(func asd => {}));
io.print(function.getArgsCount(test));
io.print(function.getArgsCount(test2));
io.print(function.getArgsCount(test3));