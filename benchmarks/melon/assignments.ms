let c = 0;
let j = 0;
let i = 0;

while(j < 10) {
    i = 0;
    while (i < 1000000) {
        let b = 1;
        c = c + b;
        i = i + 1;
    }

    j = j + 1;
}

io.print(c);