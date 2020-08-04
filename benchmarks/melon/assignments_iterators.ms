let c = 0;

for (let j in 0..10) {
    for (let i in 0..1000000) {
        let b = 1;
        c = c + b;
    }
}

io.print(c);