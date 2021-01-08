---
id: number_module
title: number
sidebar_label: number
slug: /modules/number_module
---


This module provides basic utility functions to interact with [`Number`](number.md) values.


## Functions


### fromString(strNum)
Converts a `String` to a `Number`.  


| Argument | Description |
| -------- | ----------- |
|  **strNum**  | A string representing a valid number. |



**Returns:** A valid `Number` or `null`.




### fromNumber(val)
Converts an `Integer` to a `Number` by using the rounding used by the C compiler when casting `TInteger`s to `TNumber`s. If the value to convert is already a `Number` the value is returned unmodified. If the provided value was not an `Integer` or a `Number` an error is raised.  


| Argument | Description |
| -------- | ----------- |
|  **val**  | The value to be converted to a `Number`, either an `Integer` or a `Number` |



**Returns:** A valid `Number`.





