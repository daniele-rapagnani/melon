---
id: io_module
title: io
sidebar_label: io
slug: /modules/io_module
---


This module is dedicated to input/output operations.





## Functions


### print(...)
This function prints one or more values directly to `stdout`. Each value is first converted to a string and then output. A space is used as a separator between each value. 


| Argument | Description |
| -------- | ----------- |
|  **...**  | A list of values to print |






### open(path, flags)
Opens a file and returns an object that can be used to do I/O on that file.   


| Argument | Description |
| -------- | ----------- |
|  **path**  | The path to the file to open |
|  **flags**  | The open flags, os/context dependent, such as `"w"`, `"r"`, `"w+"`, ... |



**Returns:** A valid descriptor or null




### close(file)
Closes a file that was previously open.  


| Argument | Description |
| -------- | ----------- |
|  **file**  | A file descriptor returned by `open` |



**Returns:** `true` on success, `false` otherwise




### read(file, [bytes])
Reads data from a file.   


| Argument | Description |
| -------- | ----------- |
|  **file**  | A file descriptor returned by `open` |
|  **bytes** &nbsp; <sub><sup>Optional</sup></sub>  | The maximum number of bytes to read |



**Returns:** `true` on success, `false` otherwise




### write(file, data)
Writes data to a file descriptor.   


| Argument | Description |
| -------- | ----------- |
|  **file**  | A file descriptor returned by `open` |
|  **data**  | A string with the data to be written |



**Returns:** `true` on success, `false` otherwise




### flush(file)
Flushes the buffer for a file, writing any pending changes immediately.  


| Argument | Description |
| -------- | ----------- |
|  **file**  | A file descriptor returned by `open` |



**Returns:** `true` on success, `false` otherwise




### tell(file)
Gets the current position inside a file  


| Argument | Description |
| -------- | ----------- |
|  **file**  | A file descriptor returned by `open` |



**Returns:** The number of bytes into the file




### seek(file, offset, [fromEnd])
Seeks the current position inside the file to a given value    


| Argument | Description |
| -------- | ----------- |
|  **file**  | A file descriptor returned by `open` |
|  **offset**  | An integer with the new offset in bytes |
|  **fromEnd** &nbsp; <sub><sup>Optional</sup></sub>  | If `true` sets the new position counting from the end of the file |



**Returns:** `true` on success, `false` otherwise




### size(file)
Returns the size of a given open file descriptor.  


| Argument | Description |
| -------- | ----------- |
|  **file**  | A file descriptor returned by `open` |



**Returns:** The number of total bytes for the file




### isEOF(file)
Checks if the end of file has been reached for a file descriptor.  


| Argument | Description |
| -------- | ----------- |
|  **file**  | A file descriptor returned by `open` |



**Returns:** `true` if the EOF has been reached, `false` otherwise






## Properties


### stdin
The standard input file

### stdout
The standard output file

### stderr
The standard error file

