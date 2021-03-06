let a = "asd!";
let codes = [];
let code = 0;

for (let i in 0..#a) {
    code = string.charCodeAt(a, i);
    io.print(code);
    codes[] = code;
}

let reversedCodes = [];

for (let i2 in #codes..0)
{
    reversedCodes[] = codes[i2];
}

let reconstructedString = string.fromCharCodes(reversedCodes);

io.print(reconstructedString);

// To check if the hash was calculated correctly
if (reconstructedString == "!dsa") {
    io.print("Hash matched.");
}

io.print(string.fromCharCodes(65));

a = "This is a test string";
io.print(string.find(a, "notpresent"));
io.print(string.find(a, "test"));
io.print(string.find(a, "This"));
io.print(string.find(a, "g"));
io.print(string.find(a, "test", 11));
io.print(string.replace(a, "test", "correct"));
io.print(string.replace(a, "test", "correct", 10));
io.print(string.replace(a, "test", "correct", 11));
io.print(string.replace(a, "test", "correct", null, 10));
io.print(string.replace(a, "This is a", "Those are some"));
io.print(string.replace(a, "notpresent", "Those are some"));
io.print(string.replace(a, "s", "zz"));
io.print(string.replace(a, "test ", ""));

io.print("correct" != "correcto");
io.print("" != " ");
io.print("" == "");

// case functions
io.print(string.toUpper(""));
io.print(string.toLower(""));
io.print(string.toCapitalized(""));
io.print(string.toUpper("UpPEr CaSe"));
io.print(string.toLower("LoWeR CaSe"));
io.print(string.toCapitalized(string.toLower("cApItAlIzE CaSe")));

// trim function
func testTrim |s| => {
    io.print("'" .. string.trim(s) .. "'");
}

testTrim("");
testTrim("     ");
testTrim("          ");
testTrim("          asdasd                  ");
testTrim("asdasd");

io.print(string.format("Hello World"));
io.print(string.format("Hello %s", "World"));
io.print(string.format("%-10s % 10s", "Hello", "World"));
if (string.format("Object: %p", {}) != null) {
    io.print("Pointer works");
}
io.print(string.format("Hex: %x", 10));
io.print(string.format("Hex: %X", 10));
io.print(string.format("Padded number: %015d", 10));
io.print(string.format("Float precision: %.2f", 1.12345));
io.print(string.format("Padded float: %015.2f", math.pi));

io.print(("asd")[-1:]);
io.print(("asd")[:0]);
io.print(("asd")[:2]);
io.print(("asd")[-2:2]);

let testSplit = |arr, sep| => {
    let s = "";

    if (types.isArray(arr)) {
        for (let i in arr) {
            s = s .. sep .. i;
        }

        s = s[#sep:];
    } else {
        s = arr;
    }

    io.print(s);
    io.print(json.stringify(string.split(s, sep)));
};

testSplit(["a", "b", "c"], ".");
testSplit(["abcd", "abcde", "abcdefgh", "abcdefghijk"], ".");
testSplit(["."], ".");
testSplit("", ".");
testSplit([".", "."], ".");
testSplit([".", ".abc", "."], ".");
testSplit([".", ".abc", "."], ".");
testSplit("abc", ".");
testSplit(".abc.", ".");
testSplit(["a", "@b", "@c", "@d"], "@.");
testSplit(["a", "@b", "@c", "@d"], "@@");
testSplit(["a", "@b", "@c", "@d"], "@");
testSplit(["a", "b", "c", "d"], "jklh");