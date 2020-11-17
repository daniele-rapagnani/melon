---
id: introduction
title: Introduction
sidebar_label: Introduction
slug: /
---

Melon is a dynamically typed, interpreted programming language. It attempts to combine the best features of popular high-level languages in something that feels modern but is still small and light enough to be easy to embed in your project. It's inspired mainly by *ES6 JavaScript, Lua, Wren, Python and Ruby*.

It looks like this:

```js
let Language = {
    create = |name|  => {
        return { name = name } @ Language;
    },
    
    getName = -> {
        return this.name;
    }
};

let Melon = {
    create = => {
        return { } @ Melon;
    },
    
    helloWorld = -> {
        io.print("Hello world " .. this->getName() .. "!");
    }
} @ Language.create("Melon");

let melon = Melon.create();
melon->helloWorld();
```

## Features

* A simple but functional **module system**
* Good for **soft real-time** applications such as games: it sports an **incremental generational GC**
* Native **integer** and **array** types allow for faster code
* Support for **bytecode** generation, you don't need to distribute your source files
* Full support for **operator overloading** using **ES6-style symbols**
* **Prototype based**: doesn't enforce the OOP paradigm while still making it possible
* **Dynamic typing** without implicit conversions: lower chance of unintended behaviour
* Entirely written in **C**, the number of dependencies can be counted on one hand
* Extensive **unit tests** for both the VM and language