let a = [];
let c = 0;

for (let j in 0..10) {
    for (let i in 0..500000) {
        a[] = 1;
    }
}

for (let j2 in 0..10) {
    for (let i2 in 0..500000) {
        c = c + a[i2];
    }
}

io.print(c);