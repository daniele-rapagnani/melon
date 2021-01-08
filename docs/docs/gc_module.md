---
id: gc_module
title: gc
sidebar_label: gc
slug: /modules/gc_module
---


This module exposes an interface with the VM's garbage collector.


## Functions


### trigger()
Forces the GC to be triggered right away. This call will be ignored if the GC is already running incrementally and `false` will be returned. 



**Returns:** `true` if the GC could be triggered, `false` if it couldn&#39;t.




### info(val)
Returns a formatted string with some information on the GC status of a GC managed value.  


| Argument | Description |
| -------- | ----------- |
|  **val**  | The value to inspect |



**Returns:** A string with GC related information for the provided value





