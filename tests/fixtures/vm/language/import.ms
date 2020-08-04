for (let p in import.paths) {
    io.print(p);
}

let obj = import("modules/test_module");
obj.doStuff();

let obj2 = import("modules/test_module");
obj2.doStuff();

import("modules/side_effects");
import("modules/side_effects");

import.cache = {};

import("modules/side_effects");

let moduleTest = import("modules/module_obj");
moduleTest->printInfo();

io.print("Dumping modules cache:");

for (let c in import.cache) {
    io.print(c);
    io.print(import.cache[c]);
}

import.paths = [
    "##/modules/??.mbc",
    "##/??/index.ms"
];

import.cache = {};

moduleTest = import("module_obj");
moduleTest->printInfo();

let oldImport = import[object.symbols.callOperator];

import[object.symbols.callOperator] = |path| => {
    io.print("Importing: " .. path);
    return oldImport(path);
};

import.paths = [ "##/??.ms" ];

import("modules/test_module");
import("modules/test_module");