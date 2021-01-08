---
id: object
title: Object
sidebar_label: Object
slug: /types/object
---

An `Object` is container of values where each value is associated to a key.
Values are also called properties or fields.

## Creating objects

An `Object` can be created using curly braces, for example you can create an empty object with:

```js
let myObj = {};
```

An `Object` can be initialized with some string keys/values:

```js
let myObj = {
    "myKey" = "my value",
    "Another key" = 5.0,
    "Yet another key" = true
};
```

If the key is a string made up of alphanumeric characters (plus underscores) then the double quotes can be omitted:

```js
let myObj = {
    myKey = "My Value",
    my_key2 = "My Other Value"
};
```

## Reading values

If you have the key, you can read the value associated to it by using the dot notation `.`:

```js
let myObj = { myKey = "My Value" };

io.print(myObj.myKey); // "My Value"
```

Values can also be read using the array access operator `[]`, in this case you can use any valid expression to compose the key:

```js
let myObj = { myKey = "My Value" };

io.print(myObj.myKey);              // "My Value"
io.print(myObj["myKey"]);           // "My Value"
io.print(myObj["my" .. "Key"]);     // "My Value"

let myKeyString = "myKey";
io.print(myObj[myKeyString]);       // "My Value"
```

## Writing values

Values can be written using both the dot notation `.` and the array access operator `[]`:

```js
let myObj = {};

myObj.myKey = "My Value";
io.print(myObj.myKey);              // "My Value"

myObj["myKey"] = "My Value";
io.print(myObj.myKey);              // "My Value"

myObj["my" .. "Key"] = "My Value";
io.print(myObj.myKey);              // "My Value"

let myKeyString = "myKey";
myObj[myKeyString] = "My Value";
io.print(myObj.myKey);              // "My Value"
```

## Keys from expressions

Keys can also be initialized from an expression using brackets in the key definition:

```js
let myObj = {
    ["my" .. "Key"] = "My Value",
    my_key2 = "My Other Value"
};
```

## Non-string keys

Keys are not restricted to keys only, they can also be any of the other types.
`Booleans`, `Integers`, `Numbers` and `Strings` will compare by value and can be defined using the brackets `[key]` syntax explained above:

```js
let myObj = {
    [10] = "My value",
    [(10 + 10)] = 1000.0,
    [true] = 10
};

io.print(myObject[10]); // My value
io.print(myObject[20]); // 1000.0
io.print(myObject[true]); // 10
```

## Reference keys

A reference can also be used as key in an object, such as a reference to a `Closure`, an `Array`, another `Object`, and so on. When this is done, the address of the reference will be used as the key, this means that you have to use the exact same instance to get the value back:

```js
let myObjKey = { myValue = true };
let myObj = {
    [myObjKey] = 10
};

io.print(myObj[myObjKey]); // 10

myObjKey = { myValue = true };

io.print(myObj[myObjKey]); // null

```

## Symbol keys

A [`Symbol`](symbol.md) key will match only a reference to itself. Using a `Symbol` as an object's key can be used to control the accessibility of an `Object`'s value and to avoid accidental collisions between keys:

```js
let myPrivateSymbol = $$;

let myObj = {
    [myPrivateSymbol] = "My private value"
};

// The only way to access the value is with a reference
// to the symbol just created.

io.print(myObj[myPrivateSymbol]); // My private value

myPrivateSymbol = $$;
io.print(myObj[anotherSymbol]); // null
```

:::note
This technique is used extensively throughout melon's core and modules.
One example is [operator overloading](operators.md#operators-overloading).
:::

## Iterating over keys

If you retrive the [`Iterator`](iterator.md) from an `Object` using the iterator operator `>`, you can use this iterator to iterate over the keys of an `Object`, for example in a [`for-in`](control_structures.md#for-in-loops) loop.

## Prototype

`Objects` can be chained together by making one a prototype for the other. This is done using the **bless operator** `@`:

```js
let myObjA = {};
let myObjB = {} @ myObjA;

// myObjA is the prototype of myObjB
```

When an attempt to read a non existing property is made on an object, the object's prototype (if any) will be searched for the missing property and, if found, the prototype's property's value will be returned instead of `null`.

```js
let myObjA = { a = "I'm unexpectedly here!" };
let myObjB = { b = "I'm clearly here" } @ myObjA;

io.print(myObjB.b); // I'm clearly here
io.print(myObjB.a); // I'm unexpectedly here!
```

This lookup is performed recursively on the whole prototype chain until an object with no prototype is found. If the requested key is not found on any of the prototypes the resulting value will be `null`.

:::note
This mechanism can be used to do OOP, specifically [prototype based programming](https://en.wikipedia.org/wiki/Prototype-based_programming).
:::

## Method access operator

As we know, an `Object` can contain a [`Function`](function.md) as one of its values. Most OOP languages provide the concept of a *method*, which is a function which is invoked on an object and is able to alter its internal state by using a reference to the object on which it's invoked.
In melon we can achieve a similar result with:

```js
let myObj = {
    state = 10,
    changeState = |obj| => {
        obj.state = obj.state + 1;
    }
};

myObj.changeState(myObj);
io.print(myObj.state); // 11
```

The `myObj` in the example above si clearly redundant so melon provides syntactic sugar to make it more expressive:

```js
let myObj = {
    state = 10,
    changeState = -> {
        this.state = this.state + 1;
    },
    changeState2 = |obj| => {
        obj.state = obj.state - 1;
    }
};

myObj->changeState();
io.print(myObj.state); // 11

// This still works
myObj.changeState(myObj);
io.print(myObj.state); // 12

// This is also valid
myObj->changeState2();
io.print(myObj.state); // 11
```

Basically the **method access** `->` operator silently passes the object on which it is being used as the first parameter of the closure you are accessing with it.

See also [`Function`](function.md) for a better understanding of the `this` keyword and of `->` vs `=>` when creating closures.

## Further object manipulation

There are other ways to manipulate `Object` values, they can be found in the [`object`](object_module.md) core module.