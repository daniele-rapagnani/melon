let o = {};
o[500000] = 1;

for (let i in 0..5000000) {
    if (o[i] != null) {
        io.print(i);
    }
}