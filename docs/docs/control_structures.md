---
id: control-structures
title: Control structures
sidebar_label: Control structures
slug: /types/control-structures
---

## If conditional

In melon the `if` statement is similar to most imperative programming languages,
its syntax is:

```js
if (expression) <statement>
```

The `statement` is usually a block or a single statement:

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