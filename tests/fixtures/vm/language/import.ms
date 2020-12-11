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

io.print("Number of cached modules:");
io.print(#import.cache);

import.paths = [
    "##/modules/" .. string.toString(system.getArchBits()) .. "/??.mbc",
    "##/??/index.ms",
    "##/../??/index.ms"
];

import.cache = {};

moduleTest = import("module_obj");
moduleTest->printInfo();

let oldImport = import[object.symbols.callOperator];

import[object.symbols.callOperator] = |path| => {
    io.print("Importing: " .. path);
    let importPath = null;
    let cs = debug.getCallstack();
    
    if (#cs >= 2) {
        importPath = function.getFile(cs[#cs - 2]);
    }

    return oldImport(path, importPath);
};

import.paths = [ "##/??.ms" ];

import("modules/test_module");
import("modules/test_module");

let cachedObj = import("modules/cache");
cachedObj.cached = "This value was cached";

cachedObj = import("modules/././cache");
io.print(cachedObj.cached);

import("modules/cached_user");
io.print(cachedObj.cached2);