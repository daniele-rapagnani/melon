let b = null;

for (let i in 0..1000000) {
    let a = "Lorem ipsum dolor sit amet lorem ipsum dolor sit amet";
    b = string.replace(a, "dolor", "DOLOR") .. string.toString(string.find(a, "sit"));
}

io.print(b);