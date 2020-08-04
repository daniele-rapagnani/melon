let a = 5;
let b = 6;

if (b > a)
    io.print("b > a 1");

if (b > a) {
    io.print("b > a 2");
}

if (!(a > b)) {
    io.print("!(a > b)");
}

if (a > b) { }
else io.print("a < b");

if (a == 6) {}
else if (a == 7) {}
else if (a == 5) {
    io.print("a == 5");
}

if (a > 0) {
    if (a < 7) {
        if ((a == 5) == true) {
            io.print("a == 5 2");
        }
    }
}

if (a >= 5 && b <= 6 && (a > 7 || b < 7)) {
    io.print("a >= 5 && b <= 6 && (a > 7 || b < 7)");
}

if (a > 10 && b < 6 || true) {
    io.print("a > 10 && b < 6 || true");
}