---
id: math_module
title: math
sidebar_label: math
slug: /modules/math_module
---


This module groups together basic mathematical functions.



## Functions


### sqrt(n)
 Performs the square root of `n`.  


| Argument | Description |
| -------- | ----------- |
|  **n**  | The square root argument |



**Returns:** The square root of `n`




### sin(a)
 Performs the sine function of angle `a`.  


| Argument | Description |
| -------- | ----------- |
|  **a**  | The angle in radians |



**Returns:** The sine of `a`




### cos(a)
 Performs the cosine function of angle `a`.  


| Argument | Description |
| -------- | ----------- |
|  **a**  | The angle in radians |



**Returns:** The cosine of `a`




### tan(a)
 Performs the tangent function of angle `a`.  


| Argument | Description |
| -------- | ----------- |
|  **a**  | The angle in radians |



**Returns:** The tangent of `a`




### acos(n)
 Performs the arccosine of `n`.  


| Argument | Description |
| -------- | ----------- |
|  **n**  | The argument to the arccosine function |



**Returns:** The arccosine of `n`




### asin(n)
 Performs the arcsine of `n`.  


| Argument | Description |
| -------- | ----------- |
|  **n**  | The argument to the arcsine function |



**Returns:** The arcsine of `n`




### atan(n)
 Performs the arctangent of `n`.  


| Argument | Description |
| -------- | ----------- |
|  **n**  | The argument to the arctangent function |



**Returns:** The arctangent of `n`




### atan2(x, y)
 Performs the arctangent2 of `x` and `y`.   


| Argument | Description |
| -------- | ----------- |
|  **x**  | The x coordinates of the point |
|  **y**  | The y coordinates of the point |



**Returns:** The arctangent2 of the point (`x`, `y`)




### exp(x)
 Returns e^`x` where e is the Euler number.  


| Argument | Description |
| -------- | ----------- |
|  **x**  | The exponent |



**Returns:** e^`x`




### log(x)
 Returns the natural logarithm of `x`.  


| Argument | Description |
| -------- | ----------- |
|  **x**  | The argument of the logarithm |



**Returns:** The natural logarithm of `x`




### round(x)
 Return `x` rounded.  


| Argument | Description |
| -------- | ----------- |
|  **x**  | A `Number` |



**Returns:** An `Integer`




### floor(x)
 Returns the largest integer less than or equal to `x`.  


| Argument | Description |
| -------- | ----------- |
|  **x**  | A `Number` |



**Returns:** An `Integer`




### ceil(x)
 Returns `x` rounded to the next largest integer.  


| Argument | Description |
| -------- | ----------- |
|  **x**  | A `Number` |



**Returns:** An `Integer`




### abs(x)
 Returns the absolute value of `x`.  


| Argument | Description |
| -------- | ----------- |
|  **x**  | A `Number` |



**Returns:** The absolute value of `x`




### pow(base, exp)
 Performs `base`^`exp`. This is equivalent to the [power operator](operators.md#arithmetic-operators) which is faster and thus preferable when possibile.   


| Argument | Description |
| -------- | ----------- |
|  **base**  | The exponent's base |
|  **exp**  | The exponent |



**Returns:** base^exp




### isNaN(n)
 Checks whether a given `Number` is a `NaN` value.  


| Argument | Description |
| -------- | ----------- |
|  **n**  | The `Number` to check |



**Returns:** `true` if `n` is `NaN`, `false` otherwise.




### isInfinity(n)
 Checks whether a given `Number` represents infinity.  


| Argument | Description |
| -------- | ----------- |
|  **n**  | The `Number` to check |



**Returns:** `true` if `n` represents infinity, `false` otherwise.






## Properties


### pi
Represents the ratio of the circumference of a circle to its diameter.

