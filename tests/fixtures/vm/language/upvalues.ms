
let a = [];
let x = 10;
let i = 0;

while (i < 10) {
	(func insideWhile => {	
		let j = i;
		a[] = => { return x + j; };
	})();

	i = i + 1;
}

x = 20;
i = 10;

while (i - 10 < #a) {
	io.print(string.toString(a[i - 10]()));
	i = i + 1;
}

func testPropagation |x, y| => {
	let c = x * 2;
	  
	func testPropagationInner => {
		let b = y * 2;
		return func => {
			return c + b;
		};
	}

	func testPropagationCopy |c| => {
		let b = y * 2;
		return func => {
			return c + b;
		};
	}
	  
	let fn1 = testPropagationInner();
	let fn2 = testPropagationCopy(c);
	
	c = c * 2;

	return [fn1, fn2];
}

let c = testPropagation(5, 10);

io.print(string.toString(c[0]()));
io.print(string.toString(c[1]()));