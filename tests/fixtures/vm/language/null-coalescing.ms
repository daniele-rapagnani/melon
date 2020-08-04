
io.print("test" ?? 0);
io.print(null ?? "test");
io.print(null ?? null);
io.print(null ?? null ?? "test");

let a = null;
let b = "b";

io.print(a ?? b);
io.print(5 ?? null * 10);