---
id: control-structures
title: Control structures
sidebar_label: Control structures
slug: /control-structures
---

## If conditional

In melon the `if` statement is similar to most imperative programming languages,
its syntax is:

```js
if (expression) <statement>
```

The `statement` is usually a block or a function call/assignment statement:

```js
if (1 < 2) {
    // do stuff
    io.print("Ok");
}
```

```js
if (1 < 2) io.print("Ok");
```

`else/else if` also work as usual:

```js
if (a < 2) {
    io.print("Ok");
} else if (a < 5) {
    io.print("Maybe Ok");
} else {
    io.print("Not Ok");
}
```

## While loop

While loops execute a statement while an expression is `true`, in melon they follow the syntax convention of most imperative languages:

```js
while(someExpression) <statement>
```

As in the `if` conditional, the `statement` is usually a block or a function call/assignment statement:

```js
while (someExpression) {
    // do stuff
    io.print("Ok");
}
```

```js
while (someExpression) io.print("Ok");
```

## For-in loops

The `for` loop in melon is bit different from the one in most imperative languages and it's more like a `for-each` loop. The `for` loop in melon iterates over a an [`Iterator`](iterator.md) value, assigning each element to a variable in succession.

For example you can use a `for` loop to iterate over [`Range`](range.md) values:

```js
for (let i in 0..10) {
    io.print(i);
}
```

The code above will print numbers from 0 to 9, the `for` loop will get the iterator from the `0..10` range value and put the next value in the `i` variable. On the next step of the loop, `i` will contain the next value in the iterator, `i` will change on every iteration until the iterator returns a value with its `done` property set to `true`. See [`Iterator`](iterator.md) for an in-depth explanation.