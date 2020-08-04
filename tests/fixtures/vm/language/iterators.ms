let a = [1, 2, 3, 4];

let aIt1 = >a;
let aIt2 = >a;

io.print((*aIt1).value);
io.print((*aIt1).value);
io.print((*aIt2).value);
io.print((*aIt1).value);
io.print((*aIt1).value);
io.print((*aIt2).value);

for (let v in a) {
    io.print(v);
}

let a2 = ["test string", null, 3, null, {}];

for (let v in a2) {
    io.print(v);
}

let s = "This is a string";

for (let c in s) {
    io.print(c);
}

// This requires sorting the keys before printing them
// to get a consistent output

/*let myObjWithKeys = {
    key = true,
    OtherKey = {},
    ["computed " .. string.toString(5)] = true,
    "a long string" = 10,
    [5] = 10,
    [$"mysymbol"$] = "not visible"
};

for (let key in myObjWithKeys) {
    io.print("Key = '" 
        .. string.toString(key) 
        .. "' Value = " 
        .. string.toString(myObjWithKeys[key])
    );
}*/

let MyIterable = {
    create = |values| => {
        return { values = values } @ MyIterable;
    },
    [object.symbols.iterator] = -> {
        return {
            idx = 0,
            obj = this,
            [object.symbols.nextFunction] = -> {
                if (this.idx >= #this.obj.values) {
                    return { done = true };
                }

                this.idx = this.idx + 1;

                return {
                    done = false,
                    value = "The Value Is: " .. string.toString(this.obj.values[this.idx - 1])
                };
            }
        };
    }
};

let myIt1 = MyIterable.create([1, 2, 3, 4]);
let itt = >myIt1;

for (let v2 in myIt1) {
    io.print(v2);
}