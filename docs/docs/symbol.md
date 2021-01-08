---
id: symbol
title: "Symbol"
sidebar_label: "Symbol"
slug: /types/symbol
---

A `Symbol` is a unique opaque value. When a symbol is created no other `Symbol` value will ever be equal to it.

To create a `Symbol` you can use the double dollar sign expression `$$`:

```js
let mySymbol = $$;
let mySymbol2 = $$;

io.print(mySymbol == mySymbol); // true;
io.print(mySymbol == mySymbol2); // false;
io.print($$ == $$); // false;
```

You can assign a description to the `Symbol` by passing a `String` between the `$$` expression,
this can be useful for debugging purposes:

```js
let mySymbol = $"My Symbol"$;

io.print(mySymbol);
```

Outputs:

```
[Symbol "My Symbol"]
```

:::note
melon's `Symbol` values are nothing like Ruby's symbols even though they share the same name. They are inspired by ES6 JavaScript's Symbols.
:::

## Usage

Symbols are useful to control the visibility of an `Object`'s property value, see [here](object.md#symbol-keys).