let c = [];

for (let j in 0..10) {
    for (let i in 0..1000000) {
        c[] = i;
    }
}

io.print(#c);