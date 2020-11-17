---
id: integer
title: Integer
sidebar_label: Integer
slug: /types/integer
---

An `Integer` value can be created like this:

```js
let myInteger = 10;
```

## VM Internals

The size of an `Integer` is generally a **signed 64-bit or 32-bit integer** depending on the target CPU architecture you are building for, or the interpreter build you are using. 

If you are using the standalone interpreter, running it with `--version` should reveal the integer size you are using.

:::note
Most popular interpreted languages make no distinction between floating point numbers and integer numbers. This design choice simplifies things for the programmer, but has a number of practical drawbacks such as the inability to correctly represents very big numbers, slower operation times on some processors, limited bitwise operators support. To avoid these issues Melon explicitly provides a distinct integer type.
:::