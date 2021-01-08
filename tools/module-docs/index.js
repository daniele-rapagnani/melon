const glob = require("glob");
const path = require("path");
const fs = require("fs");
const nunjucks = require("nunjucks");

const argv = require("yargs")
    .option(
        "modules-dir", {
            alias: "m",
            default: path.join(__dirname, "..", "..", "src", "melon", "modules")
        }
    )
    .option(
        "output-dir", {
            alias: "d",
            default: path.join(__dirname, "..", "..", "docs", "docs")
        }
    )
    .option(
        "name-format", {
            alias: "f",
            default: "{}_module.md"
        }
    )
    .option(
        "template", {
            alias: "t",
            default: path.join(__dirname, "template.md")
        }
    )
    .option(
        "index-file", {
            alias: "i",
            default: path.join(__dirname, "..", "..", "docs", "modules.json")
        }
    )
    .argv
;

const removeCommentAsterisks = (lines) => 
    lines.map((l) => l.replace(/^\s+\*\s+/m, "").trim())
    .filter((l) => l.length > 0)
;

const docsFromMatch = (match) => 
    removeCommentAsterisks(match.split("\n")).join("\n")
;

const getModuleDescription = (source, modPath) => {
    const re = /\/\*\*\*(.*?@module.*?)\*\//s;
    let match = re.exec(source);

    if (match === null) {
        console.warn(`${path.basename(modPath)} is missing a @module section`);
        return null;
    }

    const moduleDesc = {
        exports: []
    };

    let docs = docsFromMatch(match[1]);
    const exportsRe = /^@exports\s+([\w\.]+)\s+(.*)$/gm;

    let reMatch = null;

    while ((reMatch = exportsRe.exec(docs)) !== null) {
        moduleDesc.exports.push({
            name: reMatch[1],
            description: reMatch[2]
        });
    }

    docs = docs.replace(/@module/g, "");
    docs = docs.replace(exportsRe, "");

    moduleDesc.summary = docs;

    return moduleDesc;
};

const getCustomModuleFunctions = (source, functions) => {
    const re = /\/\*\*\*(?:(?:(?!\*\/).)*?)@funcdef\s+(\w+).*?\*\//gs;
    let funcMatch = null;

    while ((funcMatch = re.exec(source)) !== null) {
        functions.push({
            name: funcMatch[1].trim(),
            argsCount: null,
            realFuncName: null
        });
    };
};

const getModuleFunctions = (source) => {
    const re = /static\s+const\s+ModuleFunction\s+\w+\[\]\s+=\s+\{(.*?)\};/gs;
    const matches = re.exec(source);
    const functionsDefs = matches[1];

    const re2 = /\s*\{(.*?)\}\s*,?\s*/g;

    let funcMatch = null;
    const functions = [];

    while ((funcMatch = re2.exec(functionsDefs)) !== null) {
        const parts = funcMatch[1].split(",");
        const name = parts[0].trim().replace(/"/g, "");
        
        if (name == "NULL") {
            continue;
        }

        const argsCount = parseInt(parts[1].trim());
        const realFuncName = parts[3].trim().replace("&", "");

        functions.push({
            name,
            argsCount,
            realFuncName
        });
    }

    getCustomModuleFunctions(source, functions);

    return functions;
};

const getCustomFunctionDocs = (source, f) => {
    const re = new RegExp("\\/\\*\\*\\*((?:(?:(?!\\*\\/).)*?)@function\\s+" + f.name + ".*?)\\*\\/", "s");
    const match = re.exec(source);

    if (match === null) {
        return null;
    }

    return docsFromMatch(match[1]).replace(new RegExp("@function\\s+" + f.name, "sg"), "");
};

const getFunctionDocs = (source, f) => {
    const re = new RegExp("\\/\\*\\*\\*((?:(?!\\*\\/).)*?)\\*\\/[\s\n]*static\\s+TByte\\s+" + f.realFuncName + "\\s*", "s");
    const match = re.exec(source);

    let docs = null;

    if (match === null) {
        docs = getCustomFunctionDocs(source, f);

        if (docs === null) {
            console.warn(`Missing documentation for ${f.name}`);
            return null;
        }
    } else {
        docs = docsFromMatch(match[1]);
    }

    const docObj = {
        args: []
    };

    const argsRe = /^@arg\s+([\?\.\w]+)\s+(.*)$/gm;

    let reMatch = null;

    while ((reMatch = argsRe.exec(docs)) !== null) {
        docObj.args.push({
            name: reMatch[1].replace("?", ""),
            optional: reMatch[1][0] == "?",
            description: reMatch[2]
        });
    }

    docs = docs.replace(argsRe, "");

    const returnRe = /^@returns\s+(.*)$/m;

    reMatch = returnRe.exec(docs);

    if (reMatch !== null) {
        docObj.returns = {
            description: reMatch[1]
        }

        docs = docs.replace(returnRe, "");
    }

    docObj.summary = docs;
    f.docs = docObj;

    f.signature = `${f.name}(${docObj.args.map((a) => {
        if (a.optional) {
            return `[${a.name}]`;
        }

        return a.name;
    }).join(", ")})`;
};

const writeModuleDocs = (templatePath, modules, outDir, nameFormat) => {
    modules.forEach((mod) => {
        const renderedDocs = nunjucks.render(templatePath, mod);
        const fileName = nameFormat.replace("{}", mod.name);
        const fullPath = path.join(outDir, fileName);

        console.log(`Generating docs for ${mod.name} in ${fullPath}`);
        fs.writeFileSync(fullPath, renderedDocs, "utf8");
    });
};

const modules = [];

glob.sync(path.join(argv.modulesDir, "*", "*_module.c")).map(
    (modulePath) => {
        const modName = path.basename(path.dirname(modulePath));
        
        const modObj = {
            name: modName,
            id: modName.toLowerCase() + "_module"
        };

        const source = fs.readFileSync(modulePath, "utf8");
        modObj.description = getModuleDescription(source, modulePath);
        modObj.functions = getModuleFunctions(source);
    
        for (const f of modObj.functions) {
            getFunctionDocs(source, f);
        }

        modules.push(modObj);
    }
);

writeModuleDocs(argv.template, modules, argv.outputDir, argv.nameFormat);

fs.writeFileSync(
    argv.indexFile, 
    JSON.stringify(modules.map((m) => m.id)), 
    "utf8"
);