---
id: range
title: Range
sidebar_label: Range
slug: /types/range
---

A `Range` represents a consecutive range of integers.
A range can be created by using the concatenation operator `..` on two integers:

```js
let myRange = 1..10
let myOtherRange = (2 - 1)..(5 + 5);
```

This will create a range of consecutive numbers ranging from 0 to 10 (excluded).

`Range` values have a native [`Iterator`](iterator.md) that can be used to access the numbers in the range:

```js
let myRange = 1..10;
let myRangeIt = >myRange;

io.print((*myRangeIt).value); // 1
io.print((*myRangeIt).value); // 2
io.print((*myRangeIt).value); // 3
```

You can use a `for in` loop to iterate over a `Range` to get a classic for loop:

```js
for (let i in 0..5) {
    io.print(i);
}
```

Outputs:
```
0
1
2
3
4
```

Numbers in a `Range` value can also be in descending order:

```js
for (let i in 5..0) {
    io.print(i);
}
```

Outputs:
```
4
3
2
1
0
```