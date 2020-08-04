let range = 2..5;
io.print(range);

let rIt = >range;
io.print((*rIt).value);
io.print((*rIt).value);
io.print((*rIt).value);
io.print((*rIt).value);

for (let i in 0..10) {
    io.print(i);
}

for (let i2 in 10..0) {
    io.print(i2);
}

for (let i3 in 10..10) {
    io.print(i3);
}

let a = ["a", "b", "c", "d", "e"];

for (let i4 in (#a - 2)..#a) {
    io.print(a[i4]);
}