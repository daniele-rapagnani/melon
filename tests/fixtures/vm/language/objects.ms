let myEmptyObj = {};
myEmptyObj.testProp = true;

io.print(myEmptyObj.testProp);

let myObj = {
    prop1 = true,
    "another-long-prop" = 15.6
};

io.print(myObj.prop1);
io.print(myObj["another-long-prop"]);

let myNestedObj = {
    nested = {
        nestedProp = true,
        nestedMore = {
            nestedNestedProp = "yes"
        }
    },
    notNested = "another prop"
};

io.print(myNestedObj.nested);
io.print(myNestedObj.nested.nestedProp);
io.print(myNestedObj.nested.nestedMore.nestedNestedProp);
io.print(myNestedObj.notNested);

let myObjFunc = {
    func1 = func myNestedFunc => {
        return true;
    }
};

io.print(myObjFunc.func1);
io.print(myObjFunc.func1());

let myObjInt = {};
myObjInt[0] = "test int key 0";
myObjInt[1] = "test int key 1";

io.print(myObjInt[0]);
io.print(myObjInt[1]);
io.print(myObjInt["0"]);
io.print(myObjInt["1"]);

let myObjKey = {};
let myObjKey2 = {};

let myObjObj = {};

myObjObj[myObjKey] = "test object key 1";
myObjObj[myObjKey2] = "test object key 2";

io.print(myObjObj[myObjKey]);
io.print(myObjObj[myObjKey2]);
io.print(myObjObj[{}]);

let otherObj = { keyName = "external" };

let myObjExpr = {
    ["the" .. "key"] = "the value",
    [(5 + 5) * 2] = "the other value",
    [otherObj.keyName] = "external value"
};

io.print(myObjExpr.thekey);
io.print(myObjExpr[20]);
io.print(myObjExpr.external);

// object.hasKey / object.removeKey

io.print(object.hasKey(myObjExpr, "external"));
io.print(object.hasKey(myObjExpr, "invalid"));
io.print(object.removeKey(myObjExpr, "invalid"));
io.print(object.removeKey(myObjExpr, "external"));
io.print(object.removeKey(myObjExpr, "external"));
io.print(object.hasKey(myObjExpr, "external"));

let nullObjKey = { myKey = null };
io.print(object.hasKey(nullObjKey, "myKey"));
io.print(object.removeKey(nullObjKey, "myKey"));
io.print(object.hasKey(nullObjKey, "myKey"));

// object.clone

let objA = {};
let objB = objA;

io.print(objA == objB);
objB = object.clone(objA);
io.print(objA == objB);
objA = { aString = "aString", otherObj = { test = "a test", subObj = { } } };
objB = object.clone(objA);
io.print(objA == objB);
io.print(objB.aString);
io.print(objA.otherObj == objB.otherObj);
objB = object.clone(objA, true);
io.print(objA == objB);
io.print(objB.aString);
io.print(objA.otherObj == objB.otherObj);
io.print(objA.otherObj.test);
io.print(objB.otherObj.test);
io.print(objA.otherObj.subObj == objB.otherObj.subObj);
objA.otherObj.subObj.newProp = "new prop";
io.print(objA.otherObj.subObj.newProp);
io.print(objB.otherObj.subObj.newProp);

// object.merge

objA = {};
objB = {};

io.print(object.merge(objA, objB));
io.print(#objA);

objB = { test = "a test string", otherObj = { test2 = "another test" } };
io.print(object.merge(objA, objB));
io.print(#objA);
io.print(objA.test);

objB = { test = "another test string" };
io.print(object.merge(objA, objB));
io.print(#objA);
io.print(objA.test);
io.print(objA.otherObj);

objB = { otherObj = { } };
io.print(object.merge(objA, objB));
io.print(objA.otherObj.test2);
objA.otherObj = { test2 = "another test" };
objB = { otherObj = { test3 = "yet another test" } };
io.print(object.merge(objA, objB, true));
io.print(objA.otherObj.test2);
io.print(objA.otherObj.test3);
