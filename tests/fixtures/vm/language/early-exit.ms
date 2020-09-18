let f = |x| => {
    if (x > 0) {
        return x * 2;
    }

    if (x > 100) {
        return x - 100;
    }
};

io.print(f(10));
io.print(f(100));
io.print(f(-15));