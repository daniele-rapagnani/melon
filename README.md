# Melon is a small and modern programming language
Melon attempts to combine the best features of popular high-level languages in something that feels modern but is still small and light enough to be easy to embed in your project.
It's inspired mainly by ES6 JavaScript, Lua, Wren, Python and Ruby.

It looks like this:
```javascript
let Language = {
    create = |name|  => {
        return { name = name } @ Language;
    },
    
    getName = -> {
        return this.name;
    }
};

let Melon = {
    create = => {
        return { } @ Melon;
    },
    
    helloWorld = -> {
        io.print("Hello world " .. this->getName() .. "!");
    }
} @ Language.create("Melon");

let melon = Melon.create();
melon->helloWorld();
```

# Features

* A simple but functional **module system**
* Good for **soft realtime** applications such as games: it sports an **incremental generational GC**
* Native **integer** and **array** types allow for faster code
* Support for **bytecode** generation, you don't need to distribute your source files
* Full support for **operator overloading** using **ES6-style symbols**
* **Prototype based**: doesn't enforce the OOP paradigm while still making it possible
* **Dynamic typing** without implicit conversions: lower chance of unintended behaviour
* Entirely written in **C**, the number of dependencies can be counted on one hand
* Extensive **unit tests** for both the VM and language

# Where to start
Melon is very young and there's still no documentation available. 
At the moment you can get a taste for the language by browsing the [unit tests][ut].

# Compiling
Melon uses CMake so you can use the standard way of building CMake projects:

```sh
mkdir build && cd build
cmake ..
make
```

Melon is currently tested on **macOS** (*CLang*) only, but support for more platforms is a top priority.

# Disclaimer
Melon is still in **alpha** and it's certainly not ready for production!
In the unlikely event that you are considering it for your project, please keep in mind that development is very active and nothing is written in stone at the moment.

[ut]: https://github.com/daniele-rapagnani/melon/tree/develop/tests/fixtures/vm/language