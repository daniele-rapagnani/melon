io.print(null == null);
io.print(ghostVar == null);
io.print(null == ghostVar);

let ghostVar = null;
io.print(ghostVar == null);

ghostVar = "a value";
io.print(ghostVar == null);

ghostVar = {};
io.print(ghostVar == null);

ghostVar.test = "not empty anymore";
io.print(ghostVar == null);