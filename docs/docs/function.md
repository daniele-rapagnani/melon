---
id: function
title: Function
sidebar_label: Function
slug: /types/function
---

In melon functions are first-class citizens, this means that functions are a type of value and are not treated any differently from any other value. They can thus be saved inside variables, passed as arguments to other functions or stored inside an `Object` or `Array`.

## Defining functions

There are various ways to define a function in melon.

### Anonymous functions

We have seen that functions are just another type of value in melon, this means that we can assign a function to a variable as we would with any other type of value:

```js
let myFunc = => {
    io.print("This is a function");
};

myFunc();
```

Arguments in melon are expressed with pipes (`|`) when defining a function:

```js
let myFunc = |a, b, c| => {
    io.print("This is a function:", a, b, c);
}

myFunc(1, true, "foo");
```

A function can be called with more or less arguments than the one it expects.
If the arguments provided are more than the one expected the excess arguments will simply be discarded.
If less arguments are provided the arguments that were not specified will have the value `null`.

:::note
If a function does not return a value using the `return` keyword the function will still return the value `null`
:::

You can pass functions as other function's arguments:

```js
let myFunc = |a| => {
    io.print("Executing a:");
    a();
};

let myInnerFunc = => {
    io.print("It's good to be a first-class citizen!");
};

myFunc(myInnerFunc);
```

You can store functions in an `Object` or an `Array`:

```js
let myObj = {
    myFunc = => {
        io.print("I'm inside an object!");
    }
};

myObj.myFunc();
```

```js
let myArr = [
    => {
        io.print("I'm inside an array!");
        return 10;
    }
];

let myValue = myArr[0]();

io.print(myValue); // 10
```

Storing `Function` values inside an `Object` can be used to do OOP, see also [`Object`](object.md#method-access-operator) and [`Methods`](#methods)

### Labeled functions

Anonymous functions can sometime be hard to debug, especially when using `io.print` to print them:

```js
let myFunc = => { return 10; };

io.print(myFunc); // [Closure @anonymous@]
```

To make it easier to identify functions when debugging you can label them with the following syntax:

```js
let myFunc = func myFunc => { return 10; };

io.print(myFunc); // [Closure myFunc]
```

### Named functions

The most concise way to define a function is by using the named function syntax:

```js
func myFunc => {
    io.print("This is a function");
}

myFunc();
```

This is just syntactic sugar for:

```js
let myFunc = func myFunc => {
    io.print("This is a function");
}

myFunc();
```

## Closures

Closures are functions which capture values outside of the function itself:

```js
let someValue = 10;

let myFunc = |a| => {
    return a + someValue;
};

io.print(myFunc(5)); // 15
```

It's important to note that closures do not copy the value being referenced, they will simply point to the value in the stack. This means that the value used by the closure is the actual value in memory when the closure is called and not when it's defined:

```js
let someBoxedValue = 10.5;

let myFunc = => {
    return io.print(someBoxedValue);
};

someBoxedValue = 20.0f;

myFunc();
```

If the values in the stack that a closure is referencing go out of scope, the value is copied out of the stack and preserved, so that it will be available and shared among the closures referencing it.

## Methods

In OOP a method is a function that can be called on an object to alter its internal state.
Melon is not an object oriented language but it provides some syntactic sugar to make functions used as methods more expressive.

Methods could simply be declared as:

```js
let myFunc = |obj| =>  {
    obj.someState = 10;
};
```

Traditionally in object oriented languages the object on which the method has been invoked can be accessed through an implicit argument `this`.
You can do this in melon by replacing the fat arrow `=>` with `->`:

```js
let myMethod = -> {
    this.state = 10;
};

let obj = { state = 5 };
myMethod(obj);
io.print(obj.state); // 10
```

Look at the [method access operator](object.md#method-access-operator) to understand how you can use method definitions in conjunction with `Object` values to do OOP.

## Variadic arguments

Functions can consume all the arguments that are provided to them without having to specify the number of arguments they expect in advance:

```js
let myFunc = |...args| => {
    io.print("Number of arguments:", #args);
};

myFunc(); // Number of arguments: 0
myFunc(1, 2); // Number of arguments: 2
```

Standard arguments definition can also be mixed with the variadic syntax:

```js
let myFunc = |a, b, ...rest| => {
    io.print("a:", a, "b:", b, "number of rest:", #rest);
};

myFunc(10, 5); // a: 10 b: 5 number of rest: 0
myFunc(1, 2, null, null, null); // a: 1 b: 2 number of rest: 3
```

## Further function manipulation

There are other ways to manipulate `Function` values, they can be found in the [`function`](function_module.md) core module. This module includes ways to do introspection/reflection on `Function` values.