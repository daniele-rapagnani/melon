let c = 0.0;

for (let j in 0..10) {
    for (let i in 0..1000000) {
        c = c + 0.0001;
    }
}

io.print(c);