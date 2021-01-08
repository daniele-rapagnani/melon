---
id: random_module
title: random
sidebar_label: random
slug: /modules/random_module
---


This module exposes a PRNG and related functions to generate
random values from a seed.


## Functions


### seed(seed)
Sets the random seed. 


| Argument | Description |
| -------- | ----------- |
|  **seed**  | An `Integer` representing the new seed |






### getSeed()
Gets the current seed value. 



**Returns:** An `Integer` representing the current seed




### getRandomNumber([min], [max])
Generates a random `Number` value, the resulting value will be between `min` and `max` inclusive.   


| Argument | Description |
| -------- | ----------- |
|  **min** &nbsp; <sub><sup>Optional</sup></sub>  | The lower bound, defaults to 0.0 |
|  **max** &nbsp; <sub><sup>Optional</sup></sub>  | The upper bound, defaults to 1.0 |



**Returns:** A random `Number` between `min` and `max` inclusive.




### getRandomInt([min], [max])
Generates a random `Integer` value, the resulting value will be between `min` and `max` inclusive.   


| Argument | Description |
| -------- | ----------- |
|  **min** &nbsp; <sub><sup>Optional</sup></sub>  | The lower bound, defaults to the minimum negative `Integer` representable |
|  **max** &nbsp; <sub><sup>Optional</sup></sub>  | The upper bound, defaults to the maximum positive `Integer` representable |



**Returns:** A random `Integer` between `min` and `max` inclusive.





