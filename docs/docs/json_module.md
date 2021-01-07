---
id: json_module
title: json
sidebar_label: json
slug: /modules/json_module
---


This module provides built-in JSON serialization/deserialization support.


## Functions


### parse(jsonString)
Deserializes a given JSON from a string  


| Argument | Description |
| -------- | ----------- |
|  **jsonString**  | A string representing a valid JSON |



**Returns:** The deserialized JSON or `null` if parsing failed.




### parseFile(jsonPath)
Reads and parses the provided JSON file.  


| Argument | Description |
| -------- | ----------- |
|  **jsonPath**  | A valid path to a readable file |



**Returns:** The deserialized JSON or `null` if parsing failed.




### stringify(value, prettify)
Serializes a given value to a JSON string.   


| Argument | Description |
| -------- | ----------- |
|  **value**  | The value that should be JSON serialized |
|  **prettify**  | `true` If the output should be prettified, defaults to `false` |



**Returns:** A JSON string representing `value` or `null` if serialization failed.





