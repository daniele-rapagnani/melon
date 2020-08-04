let moduleInfo = this;

return {
    printInfo = -> {
        io.print(moduleInfo.path);
        this.submodule();
    },
    submodule = import("submodule")
};