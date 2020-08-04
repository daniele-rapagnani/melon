let a = [];
a[] = "my";
a[] = "first";
a[] = "array";

io.print(a[0]);
io.print(a[1]);
io.print(a[2]);

a = [];

let i = 10;

while (i > 0)
{
    a[] = i;
    i = i - 1;
}

i = 10;

while (i > 0)
{
    io.print(a[i - 1]);
    i = i - 1;
}

i = 10;

while (i > 0)
{
    a[i - 1] = a[i - 1] * 2;
    i = i - 1;
}

i = 10;

while (i > 0)
{
    io.print(a[i - 1]);
    i = i - 1;
}

let a2 = ["found", null, "found2"];

io.print(array.lookup(a2, 0));
io.print(array.lookup(a2, 1));
io.print(array.lookup(a2, 2));
io.print(array.lookup(a2, 1000));

array.resize(a2, 200);
io.print(#a2);
io.print(a2[#a2 - 1]);

let a3 = [];
io.print(array.delete(a3, 10));
io.print(array.delete(a3, 1, 10));

for (let i in 0..100) {
    a3[] = i;
}

io.print(#a3);
io.print(array.delete(a3, 1, 2000));
io.print(array.delete(a3, 2, 1));
io.print(array.delete(a3, 1));
io.print(#a3);

io.print(array.delete(a3, 10, 98));
io.print(#a3);

for (let i in 0..#a3) {
    io.print(a3[i]);
}

io.print(array.delete(a3, 0, #a3 / 2));
io.print(#a3);

for (let i in 0..#a3) {
    io.print(a3[i]);
}

for (let i in #a3..0) {
    if (i % 2 == 0) {
        array.delete(a3, i);
    }
}

for (let i in 0..#a3) {
    io.print(a3[i]);
}

io.print(array.delete(a3, 0, 1));
io.print(#a3);