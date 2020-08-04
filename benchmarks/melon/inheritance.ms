let a = {
    test = 1
};

let b = {} @ a;

let c = 0;

for (let i in 0..5000000) {
    c = c + b.test;
}

io.print(c);