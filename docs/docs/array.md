---
id: array
title: Array
sidebar_label: Array
slug: /types/array
---

An `Array` is an ordered collection of values.

## Creating arrays

An array can be created by using brackets:

```js
let myArr = []; // An empty array
```

You can also initialize an array with values:

```js
let myArr = [ 2, 5 + 5, "a string", true, null ];
```

## Reading values

You can access an `Array`'s index by using the array access operator `[]`:

```js
let myArr = [ 1, "a string", 2 ];

io.print(myArr[0]); // 1
io.print(myArr[1]); // a string
io.print(myArr[2]); // 2

```

:::note
If you access an index that's past the end of the array an error will be triggered.
This is somewhat different from popular dynamic languages, but is done intentionally to avoid undetected bugs. Melon can do this because arrays are a completely different type from `Object` and share nothing with it, while other popular programming languages use the equivalent of `Object` for arrays or an extension thereof.
:::

## Reading size

You can use the size operator `#` to retrieve an `Array`'s size:

```js
io.print(#[]); // 0
io.print(#[1, 2, 3]); // 3
```

## Adding values

You can add values to an existing array by not passing an index to the array access operator `[]` as follows:

```js
let myArr = [];
myArr[] = 1;
myArr[] = 2;
io.print(#myArr); // 2
```

## Writing values

Writing values to existing array indices works as you would expect:

```js
let myArr = [ null ];
io.print(myArr[0]); // null

myArr[0] = "it works";
io.print(myArr[0]); // it works
```

## Iterating

`Arrays` provide a native [`Iterator`](iterator.md) that can be used to iterate over them:

```js
let fruits = [ "banana", "mango", "melon" ];

for (let fruit in fruits) {
    io.print(fruit);
}
```

Outputs:

```
banana
mango
melon
```

You can also iterate over arrays using [`Range`](range.md) values, for example because the index of the item is needed:

```js
let fruits = [ "banana", "mango", "melon" ];

for (let i in 0..#fruits) {
    io.print(i, fruits[i]);
}
```

Outputs:

```
0 banana
1 mango
2 melon
```

## Array slicing

`Array` values can be sliced returning a new array composed of only a part of the original.
The slicing operator `[start:end]` can be used for slicing. 
The `start` index is inclusive and the `end` index is exclusive.
Both the `start` and `end` index are optional and the `end` index can also be a negative index expressing an index starting from the end of the array.

```js
let myArrWhole = [1, 2, 3, 4];
let myArrA = myArrWhole[1:]; // [2, 3, 4];
let myArrB = myArrWhole[:2]; // [1, 2];
let myArrC = myArrWhole[1:-1]; // [2, 3];
```

You can use the slicing operator to clone an `Array` value:

```js
let myArrWhole = [1, 2, 3, 4];
let myArrCopy = myArrWhole[:];

io.print(#myArrWhole == #myArrCopy); // true
io.print(myArrCopy == myArrWhole); // false
```

## Concatenation

You can combine two arrays in a new array by using the concatenation operator `..`:

```js
let myArrA = [ "banana", "mango" ];
let myArrB = [ "melon" ];

let myArrC = myArrA .. myArrB; // banana, mango, melon
let myArrD = myArrB .. myArrA; // melon, mango, banana
```

## Further array manipulation

There are other ways to manipulate `Array` values, they can be found in the [`array`](array_module.md) core module.