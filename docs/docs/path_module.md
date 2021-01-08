---
id: path_module
title: path
sidebar_label: path
slug: /modules/path_module
---


This module can be used to manipulate `String` values that represent
filesystem paths.


## Functions


### dirname(path)
Returns the last directory component of the provided path  


| Argument | Description |
| -------- | ----------- |
|  **path**  | The path from which the dirname will be extracted |



**Returns:** The extracted directory component or `null` on error




### basename(path)
Returns the file component of the provided path  


| Argument | Description |
| -------- | ----------- |
|  **path**  | The path from which the basename will be extracted |



**Returns:** The extracted file component or `null` on error




### realpath(path)
Resolves any relative components or symbolic links of a given path and returns the real absolute path.  


| Argument | Description |
| -------- | ----------- |
|  **path**  | The path to be transformed to a real absolute path |



**Returns:** The real path or `null` on error





