let c = "top-level c";
let b = "top-level b";

if (true) {
    let b = "test";
    io.print(b);

    {
        let b = "nested b";
        io.print(b);

        let c = "nested c";
        io.print(c);

        let inner = "inner only";
        io.print(inner);
    }

    io.print(c);
    io.print(inner);

    b = "test changed";
    io.print(b);

    let sameSlots = "value";
    io.print(sameSlots);
}

io.print(b);

{
    func myFunc => {
        io.print("This is my func");
    }

    {
        func myFunc |x| => {
            io.print("This is my inner func: " .. string.toString(x));
        }

        myFunc(10);
    }

    myFunc();

    func myOtherFunc =>  {
        io.print("My other func");
    }

    myOtherFunc();
}

for (let i in 1..5) {
    io.print("outer: " .. string.toString(i));

    for (let i in 1..5) {
        io.print("inner: " .. string.toString(i));
    }
}

for (let i in 5..0) {
    io.print(string.toString(i));
}

{
    let a = "outer a";
    let b = "outer b";

    func myFunc => {
        let b = "inner b";
        io.print(b);
        
        {
            let b = "inner inner b";
            io.print(b);
        }

        io.print(a);
        io.print(c);
    }

    myFunc();
}
