---
id: compiler_module
title: compiler
sidebar_label: compiler
slug: /modules/compiler_module
---


This module exposes access to the compiler itself and can be
used for meta-programming. This is somewhat similar to concepts
like the `eval` function in some dynamic languages except that
the code is always contained inside a new function minimizing side-effects.


## Functions


### compileFile(file, [errorCb])
Compiles a given file to a `Function`. If no error callback is provided, the interpreter will stop as if a syntax error was encoutered in one of the main source files.   


| Argument | Description |
| -------- | ----------- |
|  **file**  | The source file path |
|  **errorCb** &nbsp; <sub><sup>Optional</sup></sub>  | A callback that will be called. It will be called with `message`, `file`, `line`, `col` respectively |



**Returns:** A valid `Function` or `null`




### compile(file, [errorCb])
Compiles a given `String` to a `Function`. If no error callback is provided, the interpreter will stop as if a syntax error was encoutered in one of the main source files.   


| Argument | Description |
| -------- | ----------- |
|  **file**  | The source file path |
|  **errorCb** &nbsp; <sub><sup>Optional</sup></sub>  | A callback that will be called. It will be called with `message`, `file`, `line`, `col` respectively |



**Returns:** A valid `Function` or `null`





