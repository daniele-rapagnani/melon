let Vector = {};

Vector.create = |x, y| => {
    return { x = x, y = y } @ Vector;
};

Vector[object.symbols.sumOperator] = |other| -> {
    return Vector.create(this.x + other.x, this.y + other.y);
};

let v1 = Vector.create(0.25, 0.5);
let v2 = Vector.create(0.0, 0.0);

for (let i in 0..500000) {
    v2 = v1 + v2;
}

io.print(v2.x);
io.print(v2.y);