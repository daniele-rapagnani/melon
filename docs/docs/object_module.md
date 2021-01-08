---
id: object_module
title: object
sidebar_label: object
slug: /modules/object_module
---


This module groups together utility functions that can be used to manipulate
[`Object`](object.md) values.
It also exposes [`Symbols`](symbol.md) that can be used to customise an `Object`'s
behavior.



















## Functions


### hasKey(obj, key)
Checks if the provided object has the requested key defined. This can be used to be sure that an object has a key even if the associated value is `null`.   


| Argument | Description |
| -------- | ----------- |
|  **obj**  | The object holding the key |
|  **key**  | The key to check |



**Returns:** `true` if the key is present, `false` otherwise.




### removeKey(obj, key)
Removes a given key from an `Object`. This may cause some internal re-allocation to be triggered and it's not the same as setting a key to `null` as the key will not appear in enumeration.   


| Argument | Description |
| -------- | ----------- |
|  **obj**  | The object holding the key |
|  **key**  | The key to remove |



**Returns:** `true` if the key could be found, `false` otherwise.




### clone(obj, [deep])
Clones an `Object` returning a copy of the original. When a deep clone is performed only objects will be cloned, values which are not objects but are managed by the GC (eg: arrays) will still point to the same value and will be reference assigned.   


| Argument | Description |
| -------- | ----------- |
|  **obj**  | The object to be cloned |
|  **deep** &nbsp; <sub><sup>Optional</sup></sub>  | `true` if it should be cloned recursively |



**Returns:** The cloned object.




### merge(target, with, [deep])
Merges two objects adding to `target` any key that is missing from it but that was found in `with`.    


| Argument | Description |
| -------- | ----------- |
|  **target**  | The object that will be modified |
|  **with**  | The object with the keys to add to `target` is any is missing |
|  **deep** &nbsp; <sub><sup>Optional</sup></sub>  | `true` if the merging should be done recursively. Defaults to `false` |



**Returns:** `true` if the two objects were merged successfuly, `false` otherwise.






## Properties


### symbols.sumOperator
A symbol that can be used to overload the sum operator

### symbols.subOperator
A symbol that can be used to overload the subtraction operator

### symbols.mulOperator
A symbol that can be used to overload the multiplication operator

### symbols.divOperator
A symbol that can be used to overload the division operator

### symbols.concatOperator
A symbol that can be used to overload the concatenation operator

### symbols.compareOperator
A symbol that can be used to provide custom comparison

### symbols.getIndexOperator
A symbol that can be used to overload the indexed access operator

### symbols.setIndexOperator
A symbol that can be used to overload the indexed set operator

### symbols.negOperator
A symbol that can be used to overload the negation operator

### symbols.sizeOperator
A symbol that can be used to overload the size operator

### symbols.powOperator
A symbol that can be used to overload the power operator

### symbols.callOperator
A symbol that can be used to overload the call operator

### symbols.hashingFunction
A symbol that can be used provide custom hashing mechanism

### symbols.iterator
A symbol that can be used to overload the get iterator operator

### symbols.nextFunction
A symbol that can be used to overload the next iterator&#39;s value operator

### symbols.getPropertyOperator
A symbol that can be used to overload the dot operator for property reading

### symbols.setPropertyOperator
A symbol that can be used to overload the dot operator for property writing

