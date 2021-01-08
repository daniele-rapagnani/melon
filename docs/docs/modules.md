---
id: modules
title: "Modules"
sidebar_label: "Modules"
slug: /modules
---

Melon has a basic but flexible module system. Modules are used by the core system to expose domain specific functionality to the developer and can also be used by the developer to enhance code re-use.

## Core modules

Core modules are exposed through global variables and are pre-defined. One example is the [`io` module](io_module.md) which was used throughout this documentation.

## Importing custom modules

Custom modules must be selectively imported by the developer, to do this `import` can be called:

```js
let MyModule = import("my-module");

MyModule.doSomething();
```

:::note
As will become apparent in the sections below, `import` is not a `Function` but an `Object`.
It can be called because it has its [`call operator`](operators.md#supported-custom-operators) overriden, allowing the developer to customise
its behaviour.
:::

### Import path

The first argument of the `import` call is used to locate the module's source. How this is done depends on the content of the `import.paths` key which contains an `Array` of strings:

```js
import.paths = [
    "##/??/index.ms",
    "##/../??/index.ms"
];
```

The path strings are processed top to bottom until a valid file is found.
Special placeholders can be used inside path strings:

| Placeholder | Meaning |
| ----------- | ------- |
| `??` | The first argument of the `import` call |
| `##` | If the current call to `import` has been made from `a/b/c.ms` then `##` is `a/b` |
| `$$` | The path of the entry-point. For example if your entry point is `a/b/main.ms` then `$$` is `a/b` |

The default value for `import.paths` is:

```
##/??
##/??.ms
##/??.mbc
##/??/index.ms
##/??/index.mbc
$$/??
$$/??.ms
$$/??.mbc
$$/??/index.ms
$$/??/index.mbc
$$/mel_seeds/??.ms
$$/mel_seeds/??.mbc
$$/mel_seeds/??/index.ms
$$/mel_seeds/??/index.mbc
$$/mel_seeds/??
??
```

:::note
`import` can also import pre-compiled bytecode, that's what the `mbc` extension stands for.
:::

## Defining custom modules

Custom modules are just standard source files which `return` a value. The value will be the result of the `import` call when the module is imported:

In `a/my-module.ms`:
```js
return {
    myVar = 5,
    myFunc = => { io.print("Hello modules!"); }
};
```

In `a/some-file.ms`:
```js
let MyModule = import("my-module");

io.print(MyModule.myVar); // 5
MyModule.myFunc(); // Hello modules!
```

### Caching

Modules are cached, this means the the module's code is executed only upon first `import`. Then the returned value is cached and the same value is returned to every subsequent `import` call. This means that if your module's code has any side-effects they will be executed only once:

In `a/my-module.ms`:
```js
io.print("Importing!");

return {
    myVar = 5,
    myFunc = => { io.print("Hello modules!"); }
};
```

In `a/some-file.ms`:
```js
let MyModule = import("my-module");
let MyModule2 = import("my-module");

MyModule.myFunc(); // Hello modules!
```

Running `some-file.ms` will output:
```
Importing!
Hello modules!
```

### Accessing the cache

`import`'s cache is exposed through its `import.cache` key which is just an `Object` with the key corresponding to the real path on the filesystem of the imported file and the value to the value returned by the module. You can for example clear all `import`'s cache by simply doing:

```js
import.cache = {};
```