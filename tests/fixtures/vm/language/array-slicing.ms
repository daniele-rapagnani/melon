func printArray|a| => {
    let size = #a;
    let i = size;
    while (i > 0) {
        io.print(a[size - i]);
        i = i - 1;
    }
    io.print("---");
}

let a = [1, 2, 3, 4];
let a1 = a[:];
let a2 = a[2:];
let a3 = a[:1];
let a4 = a[:-2];
let a5 = a[-2:-1];
let a6 = a[2:1];
let a7 = a[10:15];

let c = 2;

func plusOne|v| => { return v + 1; }

let a8 = a[c - 1:plusOne(c)];

printArray(a);
printArray(a1);
printArray(a2);
printArray(a3);
printArray(a4);
printArray(a5);
printArray(a6);
printArray(a7);
printArray(a8);

printArray(a[0:1] .. a[-3:-2]);
printArray(a[:] .. a[:] .. a[:]);

a = a[:] .. a .. a;

printArray(a);