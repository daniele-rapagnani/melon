---
id: string_module
title: string
sidebar_label: string
slug: /modules/string_module
---


This module provides functions for basic string manipulation.


## Functions


### toString(val)
Converts the provided value to its `String` representation.  


| Argument | Description |
| -------- | ----------- |
|  **val**  | The value to convert to string |



**Returns:** The string representation of `val`




### charCodeAt(str, idx)
Returns an `Integer` representing the ASCII code of the character at index `idx` in `str`.   


| Argument | Description |
| -------- | ----------- |
|  **str**  | The subject string |
|  **idx**  | The character's index |



**Returns:** An `Integer` or `null` if the index is out of range




### fromCharCodes(codes)
Creates a string from a list of ASCII codes.  


| Argument | Description |
| -------- | ----------- |
|  **codes**  | A single `Integer` or an `Array` of `Integer` values. Any non-`Integer` entry will be skipped. |



**Returns:** A new string made up of the provided sequence of ASCII codes




### replace(haystack, needle, replacement, [start], [end])
Replaces with `replacement` any occurrence of `needle` found inside `haystack` between the `start` (inclusive) and `end` (exclusive) indices.      


| Argument | Description |
| -------- | ----------- |
|  **haystack**  | The string in which to find the `needle` |
|  **needle**  | The substring to be replaced by `replacement` |
|  **replacement**  | The string to replace any occurrence of `needle` |
|  **start** &nbsp; <sub><sup>Optional</sup></sub>  | The index of `haystack` at which the search should start, defaults to 0 |
|  **end** &nbsp; <sub><sup>Optional</sup></sub>  | The index of `haystack` at which the search should end, defaults to the length of `haystack` |



**Returns:** A new string with `needle` replaced by `replacement` or the original `haystack` if `needle` was not found.




### find(haystack, needle, [start])
Looks for any occurrence of `needle` in `haystack` starting from the `start` index.    


| Argument | Description |
| -------- | ----------- |
|  **haystack**  | The string in which to find the `needle` |
|  **needle**  | The substring to search for |
|  **start** &nbsp; <sub><sup>Optional</sup></sub>  | The index at which the search should start, defaults to 0 |



**Returns:** An `Integer` with the index in `haystack` at which the first character of `needle` was found. `null` if `needle` couldn&#39;t be found.




### toLower(str)
Transforms the provided string to lowercase.  


| Argument | Description |
| -------- | ----------- |
|  **str**  | The string to transform |



**Returns:** The transformed string




### toUpper(str)
Transforms the provided string to uppercase.  


| Argument | Description |
| -------- | ----------- |
|  **str**  | The string to transform |



**Returns:** The transformed string




### toCapitalized(str)
Capitalizes the provided string, only the first character is transformed to uppercase.  


| Argument | Description |
| -------- | ----------- |
|  **str**  | The string to transform |



**Returns:** The transformed string




### trim(str)
Trims a string removing any space character from the beginning and end of a string.  


| Argument | Description |
| -------- | ----------- |
|  **str**  | The string to be trimmed |



**Returns:** The trimmed string




### format(fmt, args)
Formats a string with [printf](http://www.cplusplus.com/reference/cstdio/printf/) style formatting.   


| Argument | Description |
| -------- | ----------- |
|  **fmt**  | The format to be used when formatting the string |
|  **args**  | An array of the values required by the `fmt` string, it may be an empty array |



**Returns:** The values provided in `args` formatted using `fmt`




### split(str, token)
Splits `str` using `token` as a delimiter.   


| Argument | Description |
| -------- | ----------- |
|  **str**  | The string to be splitted |
|  **token**  | The substring to use as delimiter |



**Returns:** An array with the strings resulted from splitting `str` by `token`.





