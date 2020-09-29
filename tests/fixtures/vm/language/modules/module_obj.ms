let moduleInfo = this;

return {
    printInfo = -> {
        io.print(path.basename(moduleInfo.path));
        this.submodule();
    },
    submodule = import("submodule")
};