
func test |...args| => {
    for (let i in args) {
        io.print(i);
    }
}

test("first");
test("first", "second");
test("first", "second", null, 3);
test();

func arity |...args| => {
    io.print(#args);
}

arity();
arity(1);
arity(1, 2);
arity(1, 2, 3);
arity(1, 2, 3, 4);

func multiArgs |a, b, ...args| => {
    io.print(a);
    io.print(b);

    for (let i in args) {
        io.print(i);
    }
}

multiArgs();
multiArgs("a", "b");
multiArgs("a", "b", "varg", "varg", "varg");

func forward |...args| => {
    function.call(multiArgs, args);
}

forward("a", "b", "varg1", "varg2", "varg3", "varg4");

func sum |a, b| => {
    return a + b;
}

func bind |fn, ...boundArgs| => {
    return |...args| => {
        return function.call(fn, boundArgs .. args);
    };
}

let sum10 = bind(sum, 10);
let sum10and5 = bind(sum, 10, 5);

io.print(sum10(5));
io.print(sum10and5());