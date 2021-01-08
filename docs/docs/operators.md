---
id: operators
title: Operators
sidebar_label: Operators
slug: /operators
---

## Supported operators

### Arithmetic operators

| Operator           | Type   | Description          |
| ------------------ | ------ | -------------------- |
| `+`                | Binary | Sum                  |
| `-`                | Binary | Subtraction          |
| `-`                | Unary  | Negation             |
| `/`                | Binary | Division             |
| `*`                | Binary | Multiplication       |
| `%`                | Binary | Modulo               |
| `^`                | Binary | Power                |

### Boolean operators

| Operator           | Type   | Description          |
| ------------------ | ------ | -------------------- |
| `&&`               | Binary | Logic And            |
| <code>\|\|</code>  | Binary | Logic Or             |
| `!`                | Unary  | Logic Not            |

### Comparison operators

| Operator           | Type   | Description          |
| ------------------ | ------ | -------------------- |
| `==`               | Binary | Equality             |
| `!=`               | Binary | Inequalit            |
| `>`                | Binary | Greater              |
| `>=`               | Binary | Greater or equal     |
| `<`                | Binary | Less                 |
| `<=`               | Binary | Less or equal        |

### Concatenation operator

| Operator           | Type   | Description          |
| ------------------ | ------ | -------------------- |
| `..`               | Binary | Concatenation        |

This operator is an alternative sum operator that can be used
to join for example [`String`](string.md) or [`Array`](array.md) values.

### Bless operator

| Operator           | Type   | Description          |
| ------------------ | ------ | -------------------- |
| `@`                | Binary | Change prototype     |

This operator can be used to change the prototype of an
[`Object`](object.md#prototype).

### Iterator operators

| Operator           | Type   | Description          |
| ------------------ | ------ | -------------------- |
| `>`                | Unary  | Get value's iterator |
| `*`                | Unary  | Advance iterator     |

### Bitwise operators

| Operator           | Type   | Description          |
| ------------------ | ------ | -------------------- |
| `&`                | Binary | And                  |
| <code>\|</code>    | Binary | Or                   |
| `>>`               | Binary | Right shift          |
| `<<`               | Binary | Left shift           |
| `^^`               | Binary | Xor                  |
| `^`                | Unary  | Not                  |

### Size operator

| Operator           | Type   | Description                 |
| ------------------ | ------ | --------------------------- |
| `#`                | Unary  | Get the the size of a value |

This operator can be used to retrieve the size of some value,
see for example [`String`](string.md#string-length) or [`Array`](array.md#reading-size).

## Binary operators precedence

Operators with higher precedence are processed as operands of operators with lower precedence.
Lower numbers mean higher precedence.

| Precedence  | Operators             |
| ----------- | --------------------- |
| 1           | `.`                   |
| 2           | `%` `/` `*`           |
| 3           | `+` `-` `^^`          |
| 4           | `>>` `<<`             |
| 5           | `>` `<` `>=` `<=` `@` |
| 6           | `==` `!=` `..`        |
| 7           | `&`                   |
| 8           | `^`                   |
| 9           | <code>&#124;</code>   |
| 10          |  `&&`                 |
| 11          | \|\|                  |
| 12          |  `??`                 |

As an example:
```js
5 * 4 + 3 >> 2 & 3
```

Would be processed as:
```js
(((5 * 4) + 3) >> 2) & 3
```

## Operators overloading

Melon supports operator overloading, this means that you can add your custom logic to be executed when an operator is used on [`Object`](object.md) operands.

To overload an operator, [`Symbol` keys](object.md#symbol-keys) are used. Each operator has its own key which is stored inside the [`object`](object_module.md) core module.

To perform the overload you can simply store a `Function` or [`method`](function.md#methods) with the custom operator's logic at its corresponding `Symbol` key:

```js
let MyObj = {
    create = => {
        return {} @ MyObj;
    },
    [object.symbols.mulOperator] = |other| => {
        return other * other;
    }
};

let a = MyObj.create();

io.print(a * 5); // 25
```

As can been seen in the example above, you can combine [`Object` prototypes](object.md#prototype) with operator overloading to overload one or more operators for all objects which extend a given prototype. This is somewhat akin to overloading operators for all instances of a class in traditional OOP.

### Custom operator's function

When writing a custom operator's function you should keep in mind which arguments you are going to get, as well as the value you are going to return.

For unary operators there's only a single value, the operand. 
For binary operators you are going to get two argument with both operands.

The result of the operator applied on the operand(s) must be returned from your function.
This is true for most operators except for example the `setPropertyOperator` or the `setIndexOperator`.

All custom operator's functions can return the value `false` to abort the custom operator and simply try to fallback to the default behaviour.

This is useful for example in conjunction with the `*PropertyOperator` or `*IndexOperator`:

```js
let myCustomProp = {
    [object.symbols.getPropertyOperator] = |prop| -> {
        if (prop == "test") {
            return "dynamic value";
        }
        return false;
    },
    [object.symbols.setPropertyOperator] = |prop, val| -> {
        io.print(prop);
        io.print(val);
        if (prop == "test2") {
            return true;
        }

        return false;
    }
};
```

You can also overload the `setPropertyOperator` and `setIndexOperator` to create objects which can not be modified from the outside:

```js
let readOnlyObj = {
    test = "this is read only",
    [object.symbols.setPropertyOperator] = -> {
        return true;
    }
};
```

:::note
Considering the the first argument in all operators is the left hand side of a binary operand or the unary operand, you can use [`methods`](function.md#methods) to define a custom operator's behaviour and just use the `this` argument to refer to the first operand.
:::

### Supported custom operators

| Symbol | Operator |
| -----  | ----------- |
|`object.symbols.sumOperator` | `+` |
|`object.symbols.subOperator` | `-` |
|`object.symbols.mulOperator` | `*` |
|`object.symbols.divOperator` | `/` |
|`object.symbols.concatOperator` | `..` |
|`object.symbols.compareOperator` | This can be used to implement `==`, `<`, `<=`, `>` and `>=`. The operator's `Function` should return an integer with the result of the `left_hand_value - right_hand_value` expression |
|`object.symbols.getIndexOperator` | `obj[i]` where `i` is an integer |
|`object.symbols.setIndexOperator` | `obj[i] = <expr>` where `i` is an integer |
|`object.symbols.negOperator` | Unary `-` |
|`object.symbols.sizeOperator` | `#` |
|`object.symbols.powOperator` | `^` |
|`object.symbols.callOperator` | `obj()`, can be used to implement functors |
|`object.symbols.hashingFunction` | Can be used to supply custom hashing for an object, to be used as key of another `Object`. The operator's `Function` should return an integer representing the hash. |
|`object.symbols.iterator` | Unary `>` |
|`object.symbols.nextFunction` | Unary `*` |
|`object.symbols.getPropertyOperator` | `obj.k` |
|`object.symbols.setPropertyOperator` | `obj.k` |  