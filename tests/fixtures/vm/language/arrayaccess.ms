let a = [1, 2, 3, 4];
io.print(a[2]);

let obj = { testProp1 = true, testProp2 = "correct" };
io.print(obj["testProp1"]);
io.print(obj["testProp" .. string.toString(a[1])]);

let str = "thisIsMyString";
io.print(str[7]);
io.print(str[0]);

a[2] = a[2] + 1;
io.print(a[2]);

obj["testProp1"] = !obj["testProp1"];
io.print(obj.testProp1);