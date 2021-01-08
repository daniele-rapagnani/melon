---
id: array_module
title: array
sidebar_label: array
slug: /modules/array_module
---


This module defines utility functions for [`Array`](array.md) values manipulation.


## Functions


### resize(arr, size)
Ensures the size of an array is at least the one provided.  


| Argument | Description |
| -------- | ----------- |
|  **arr**  | The array to resize |
|  **size**  | An integer with the new size |






### lookup(arr, index)
Returns the value stored at a given index for a given array without rising any error if the index does not exist.   @return The requeste value if present, `null` otherwise


| Argument | Description |
| -------- | ----------- |
|  **arr**  | The array |
|  **index**  | The index to lookup |






### delete(arr, start, end)
Removes one or more elements from an array, resizing it if necessary.    


| Argument | Description |
| -------- | ----------- |
|  **arr**  | The array |
|  **start**  | The index of the first element to remove |
|  **end**  | The index of the last element to remove |



**Returns:** `true` on success, `false` otherwise





