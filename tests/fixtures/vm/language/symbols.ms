let mySymbol = $$;
let mySymbolLabel = $"my symbol"$;

io.print(mySymbol);
io.print(mySymbolLabel);
io.print(mySymbol == mySymbol);
io.print(mySymbolLabel == mySymbolLabel);
io.print(mySymbol == mySymbolLabel);
io.print($$ == $$);
io.print($"same name"$ == $"same name"$);

let anotherVar = mySymbol;

io.print(anotherVar == mySymbol);

let myKeyA = $"the key"$;
let myKeyB = $"the key"$;

let myObj = {};

myObj[myKeyA] = "the value of key a";
myObj[myKeyB] = "the value of key b";

io.print(myObj[myKeyA]);
io.print(myObj[myKeyB]);

let myObj2 = {
    [$$] = "lost forever",
    [myKeyA] = "another value of key a"
};

io.print(myObj2[$$]);
io.print(myObj2[myKeyA]);