---
id: number
title: Number
sidebar_label: Number
slug: /types/number
---

A `Number` value can represent a floating point number:

```js
let myNumber = 10.0;
myNumber = 0.005;
myNumber = -5.456e3;
myNumber = 5e-5;
```

To distinguish a `Number` value from an `Integer` value, the decimal **dot separator** should always be used:

```js
let myInteger = 10;
let myNumber = 10.0;
```

## Mixing numbers and integers

Combining `Number` values and `Integer` values with basic operations work as you would expect: `Integer` values are promoted to `Number` values and the result is a `Number` value.
However comparing `Integer`s and `Number`s is not allowed to avoid any ambiguity, you have to convert one to the other:

```js
// Compares an integer and a number
if (5 * 1.0 > 4.0) {
}

// Compares a number and an integer, rounding the integer
if (math.round(5.6) > 4) {
}

// Compares a number and an integer discarding the fractional part
if (math.floor(5.6) > 4) {
}
```

## VM Internals

A `Number` is internally represented as **double-precision 64-bit IEEE 754**.