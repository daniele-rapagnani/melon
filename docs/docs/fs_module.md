---
id: fs_module
title: fs
sidebar_label: fs
slug: /modules/fs_module
---


This module exposes functions for filesystem manipulation.


## Functions


### exists(path)
 Checks whether a given path exists on file system.  


| Argument | Description |
| -------- | ----------- |
|  **path**  | The path to check |



**Returns:** `true` if the provided path exists, `false` otherwise




### isFile(path)
 Checks whether a given path corresponds to a valid file on filesystem.  


| Argument | Description |
| -------- | ----------- |
|  **path**  | The path to check |



**Returns:** `true` if the provided path is a file, `false` otherwise




### isDirectory(path)
 Checks whether a given path corresponds to a directory on filesystem.  


| Argument | Description |
| -------- | ----------- |
|  **path**  | The path to check |



**Returns:** `true` if the provided path is a directory, `false` otherwise




### isReadable(path)
 Checks whether a given path is readable.  


| Argument | Description |
| -------- | ----------- |
|  **path**  | The path to check |



**Returns:** `true` if the provided path is readable, `false` otherwise




### isWritable(path)
 Checks whether a given path is writable.  


| Argument | Description |
| -------- | ----------- |
|  **path**  | The path to check |



**Returns:** `true` if the provided path is writable, `false` otherwise





