---
id: string
title: "String"
sidebar_label: "String"
slug: /types/string
---

A `String` value represents a sequence of characters.
A string can be created by wrapping the characters in double quotes:

```js
let myStringVar = "Hello, world!";
```

Strings are immutable, this means that whenever you want to modify a string you'll get a new one and the original one will remain unchanged.

## String length

You can use the size operator `#` to retrive the size of a `String`:

```js
let myString = "12345";
io.print(#myString); // 5
io.print(#"") // 0
```

## Strings concatenation

A `String` can be created by concatenating two other `String` values by using the concatenation operator `..`:

```js
let stringA = "Hello, ";
let stringB = "world!";
let stringC = stringA .. stringB;

io.print(stringC); // Hello, world!
io.print(stringA .. stringA .. stringC); // Hello, Hello, Hello, world!
```

## Accessing individual characters

The indexing operator can be used to read individual characters in a `String`:

```js
let stringA = "Hello";

io.print(stringA[0]); // H
io.print(stringA[1]); // e
io.print(stringA[2]); // l
io.print(stringA[3]); // l
```

:::note
Given that strings are immutable, you can't change a character by using indexed assignment.
:::

##  Accessing with ranges

You can use index ranges to extract a portion of a string, 
much as you would with an [`Array`](array.md) value:

```js
let stringA = "Hello";

io.print(stringA[0:3] .. "!"); // Hell!
io.print(stringA[:-1] .. "!"); // Hell!
io.print(stringA[-2:]); // lo
```

## String manipulation

Most string manipulation functions can be found in the [`string`](string_module.md) module.

## VM Internals

### Internalization

Strings are [internalized](https://en.wikipedia.org/wiki/String_interning). This means that up to a certain length, if the value is the same, the same exact string in memory will be used for each content. This makes comparing small strings very fast:

```js
let myShortString = "short";

// This is fast!
if (myShortString == "short") {
}
```

In the example above the VM will simply compare the memory location of the two strings, a simple integer comparison, knowing that the string `"short"` is internalized. If the strings are the same their location in memory must match.

### Wide-string support

Currently wide strings are not supported. This means that while you can output multi-byte encoded character (such as emojis), Melon will see the multi-byte components as distinct characters.
While this may not be a big issue in some cases, it's something to be aware of when doing string manipulation as it may break the string (eg: a multi-byte sequence is interrupted).

This is also why if you use the size operator `#` on a multi-byte char it will not return `1`.