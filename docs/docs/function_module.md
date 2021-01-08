---
id: function_module
title: function
sidebar_label: function
slug: /modules/function_module
---


This module can be used to programmatically interact with [`Function`](function.md)
values. Most of the functions in this module can be built upon to enable
basic functional programming patterns.


## Functions


### call(func, args)
Calls a function value programmatically   


| Argument | Description |
| -------- | ----------- |
|  **func**  | The function to be invoked |
|  **args**  | An array of arguments with which the function will be invoked |



**Returns:** Whatever the original function returned




### getName(func)
Gets the name of a function value  


| Argument | Description |
| -------- | ----------- |
|  **func**  | The function to extract the name from |



**Returns:** The function&#39;s name if any, `null` otherwise




### getArgsCount(func)
Gets the number of arguments a given function expects  


| Argument | Description |
| -------- | ----------- |
|  **func**  | The function to inspect |



**Returns:** An integer representing the number of expected arguments




### getFile(func)
Returns the path to the file in which a given function value was defined. If the string is built-in or was defined in some non-standard way `null` will be returned.  


| Argument | Description |
| -------- | ----------- |
|  **func**  | The function to inspect |



**Returns:** A string containing the path to the file or `null`





