---
id: variables
title: Variables
sidebar_label: Variables
slug: /variables
---

Melon is a dynamically typed language. This means that the programmer doesn't have to specify
the type of a variable in advance, instead a variable can hold any type of value at any given moment of the program's execution.

## Local

Local variables are declared using the `let` keyword followed by the **variable name**.

Variable names can have any combination of number, letters and the character `_` but they **cannot** start with a number.

```js
let myVariable = "Hello";
```

Variables declared with `let` are block-scoped:

```js
let myVariable = "Hello";

if (something) {
    let myVariable = "I'm alive only inside this block!";
}
```

## Global

Any variable that is read or written to without having been declared is considered to be a global variable:

```js
myGlobalVar = "I'm inside a global variable";
```

Global variables are shared by the entire **VM** instance and can be accessed from anywhere.

The language uses global variables to expose core modules to the programmer, such as the `io` or `fs` module.