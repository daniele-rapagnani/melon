---
id: iterator
title: Iterator
sidebar_label: Iterator
slug: /types/iterator
---

An `Iterator` value is something that can be used to get a sequence of other values by abstracting the actual way used to calculate them.

## Getting iterators

An `Iterator` value can't be created directly, but it can be created from another value by using the iterator operator `>`:

```js
let a = [1, 2, 3];
let aIt = >a;
```

If the value we are using the iterator operator on doesn't provide an iterator, the operator will return `null`:

```js
io.print(>5); // null
```

## Advancing iterators

The next operator `*` can be used on `Iterator` values to get the current value and advance the `Iterator` to the next value.

The next operator returns an object with two keys: `value` and `done`. The `value` property will contain the current value and the `done` property will be `true` if there are no more values left for the iterator to generate:

```js
let a = [1, 2];
let aIt = >a;

let v = *aIt;
io.print(v.value, v.done);

v = *aIt;
io.print(v.value, v.done);

v = *aIt;
io.print(v.value, v.done);
```

Outputs:

```
1 false
2 false
null true
```

:::note
The use of a separate `done` value allows for `Iterators` to also return `null` values without any ambiguity.
:::

## Cycling

`Iterator` values can be used in conjunction with the `for in` loop to iterate over the values of a finite `Iterator`:

```js
let a = [1, 2];

for (let item in a) {
    io.print(item);
}
```

Outputs:
```
1
2
```