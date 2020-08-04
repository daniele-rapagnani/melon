
let a = false && invalid();
let c = true || invalid();

io.print(a);
io.print(c);

let t = true;
let f = false;

io.print(f && invalid() || t);
io.print(f && invalid() && t && (t || invalid()));
io.print(f || (f && invalid()));

let obj = {
    test = f && invalid() || t,
    [f && invalid()] = "correct"
};

io.print(obj.test);
io.print(obj[false]);

if (f && invalid() || t) {
    io.print("if correct");
}

while (f && invalid()) {
    io.print("this should not print");
}

func fn => {
    return f && invalid() || t;
}

io.print(fn());