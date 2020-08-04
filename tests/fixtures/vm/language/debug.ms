func printStackNames => {
    for (let f in debug.getCallstack()) {
        io.print(f);
    }
}

func funcB => {
    printStackNames();
}

func funcA => {
    funcB();
}

funcA();

func funcAR |c| => {
    if (c == 0) {
        printStackNames();
    } else {
        funcAR(c - 1);
    }
}

funcAR(5);