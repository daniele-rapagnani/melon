let c = 0;

func f => {
    return 1;
}

for (let j in 0..10) {
  for (let i in 0..1000000) {
        c = c + f();
    }
}

io.print(c);