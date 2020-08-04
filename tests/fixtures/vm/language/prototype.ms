let Animal = {
    name = null,
    create = |name| => {
        return { name = name };
    },
    walk = -> {
        let walkingStr = "Walking";
        io.print(walkingStr .. " " .. this.name);
    },
    introduce = func intro |otherName| -> {
        io.print("Hey " .. otherName .. ", this is " .. this.name .. "!");
    }
};

let Dog = {
    create = |name| => {
        return Animal.create(name) @ Dog;
    },

    bark = -> {
        io.print("Woof!!");
    }
} @ Animal;

let Shiba = {
    create = |name| => {
        return Dog.create(name) @ Shiba;
    },

    bark = -> {
        io.print("Woof with pride!!");
    }
} @ Dog;

func dogRoutine|dog| => {
    dog->walk();
    dog->introduce("Tester");
    dog->bark();
    dog.bark(dog);
}

let myDog = Dog.create("Fido");
let myShiba = Shiba.create("Shishi");

dogRoutine(myDog);
dogRoutine(myShiba);

io.print(@myDog == Dog);
io.print(@myShiba == Dog);
io.print(@myShiba == Shiba);
io.print(@@myShiba == Dog);

let unblessedObject = {};
io.print(@unblessedObject);
io.print(@{});
