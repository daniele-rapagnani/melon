---
id: debug_module
title: debug
sidebar_label: debug
slug: /modules/debug_module
---


This module groups some debugging utility functions together. It can be
used to inspect the VM or build more sophisticated debugging
functionalities on top of it.


## Functions


### printStack()
Prints the current stack to `stdout`






### printCallstack()
Prints the current call stack to `stdout`






### error(message)
Prints an error and aborts the program's execution. 


| Argument | Description |
| -------- | ----------- |
|  **message**  | The error message |






### getCallstack()
This function returns an array of the function objects which are currently in the call stack. 



**Returns:** An array of functions, the top of the stack is at index `0`




### dump(val)
Dumps a value to `stdout` using the internal dump function 


| Argument | Description |
| -------- | ----------- |
|  **val**  | The value to dump |







