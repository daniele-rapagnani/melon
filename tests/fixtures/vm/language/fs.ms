
let funcs = [
    "exists",
    "isFile",
    "isDirectory",
    "isReadable",
    "isWritable"
];

func listPerms |path| => {
    for (let f in funcs) {
        io.print(f .. "(" .. path .. ")" .. " = " .. string.toString(fs[f](path)));
    }
}

listPerms("wrong");
listPerms("fixtures/vm/language/fs_test_files");
listPerms("fixtures/vm/language/fs_test_files/a_file.txt");