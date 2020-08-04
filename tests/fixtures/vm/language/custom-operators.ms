let Vector = {
    create = |x, y| => {
        return { x = x, y = y } @ Vector;
    },
    [object.symbols.sumOperator] = |other| -> {
        return Vector.create(this.x + other.x, this.y + other.y);
    },
    [object.symbols.subOperator] = |other| -> {
        return Vector.create(this.x - other.x, this.y - other.y);
    },
    [object.symbols.mulOperator] = |other| -> {
        return Vector.create(this.x * other, this.y * other);
    },
    [object.symbols.divOperator] = |other| -> {
        return Vector.create(this.x / other, this.y / other);
    },
    [object.symbols.sizeOperator] = -> {
        return math.sqrt(this.x * this.x + this.y * this.y);
    },
    [object.symbols.negOperator] = -> {
        return Vector.create(-this.x, -this.y);
    },
    [object.symbols.getIndexOperator] = |idx| -> {
        if (idx == 0)
        {
            return this.x;
        }
        else if (idx == 1)
        {
            return this.y;
        }

        return false;
    },
    [object.symbols.setIndexOperator] = |idx, val| -> {
        if (!types.isInteger(idx))
        {
            return false;
        }

        if (idx == 0)
        {
            this.x = val;
        }
        else if (idx == 1)
        {
            this.y = val;
        }

        return idx == 0 || idx == 1;
    },
    [object.symbols.compareOperator] = |other| -> {
        if (this.x == other.x && this.y == other.y)
        {
            return 0;
        }

        return math.round(#this - #other);
    },
    [object.symbols.powOperator] = |other| -> {
        return this.x * other.x + this.y * other.y;
    }
}; 

let v1 = Vector.create(10.0, 10.0);
let v2 = Vector.create(5.0, 8.0);

let v3 = v1 + v2;
let v4 = v1 - v2;
let v5 = v2 * 4.0;
let v6 = v2 / 4.0;
let v7 = -v6;

io.print(v3.x);
io.print(v3.y);

io.print(v4.x);
io.print(v4.y);

io.print(v5.x);
io.print(v5.y);

io.print(v6.x);
io.print(v6.y);

io.print(v7.x);
io.print(v7.y);

io.print(#v7);

io.print(v7[0]);
io.print(v7[1]);

v1[0] = 10.0;
v1[1] = 5.0;

io.print(v1.x);
io.print(v1.y);

v1["x"] = 20.0;

io.print(v1.x);
io.print(v1.y);

io.print(v1 ^ v2);

io.print(v6 < v7);
io.print(v6 > v7);
io.print(v6 == v7);

io.print(v3 < v1);
io.print(v3 > v1);
io.print(v3 == v1);

let MyObj = {
    create = => {
        return {} @ MyObj;
    },
    [object.symbols.mulOperator] = |other| => {
        return other * other;
    }
};

let a = MyObj.create();

io.print(a * 5);

let NumberObj = {
    create = |n| => {
        return { value = n } @ NumberObj;
    },
    [object.symbols.compareOperator] = |other| -> {
        return this.value - other.value;
    },
    [object.symbols.hashingFunction] = -> {
        return this.value;
    }
};

let no = NumberObj.create(2);
let no2 = NumberObj.create(2);
let no3 = NumberObj.create(3);

let myObjKeyed = {};
myObjKeyed[no] = "this should be printed";
io.print(myObjKeyed[no2]);
io.print(myObjKeyed[no3]);

let FunctorObj = {
    create = |num| => {
        return { num = num } @ FunctorObj;
    },
    [object.symbols.callOperator] = |yourName, num| -> {
        io.print("Hello " .. yourName .. " your number is: " .. string.toString(this.num * num));
        return (this.num ^ 2) * num;
    }
};

let myFunc = FunctorObj.create(5);
let res = myFunc("Sporty", 10);
io.print(res);

let staticFunctor = {
    [object.symbols.callOperator] = |num| -> {
        return num * num;
    }
};

io.print(staticFunctor(5));

let myCustomProp = {
    [object.symbols.getPropertyOperator] = |prop| -> {
        if (prop == "test") {
            return "dynamic value";
        }
        return false;
    },
    [object.symbols.setPropertyOperator] = |prop, val| -> {
        io.print(prop);
        io.print(val);
        if (prop == "test2") {
            return true;
        }

        return false;
    }
};

io.print(myCustomProp.test);
io.print(myCustomProp.anotherProp);

myCustomProp.randomProp = "random value";
myCustomProp.test2 = "should not assign";

io.print(myCustomProp.randomProp);
io.print(myCustomProp.test2);

let readOnlyObj = {
    test = "this is read only",
    [object.symbols.setPropertyOperator] = -> {
        return true;
    }
};

io.print(readOnlyObj.test);
readOnlyObj.test = "no way";
io.print(readOnlyObj.test);
readOnlyObj.ghostProp = "never going to assign";
io.print(readOnlyObj.ghostProp);