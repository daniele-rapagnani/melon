io.print(1 << 8);
io.print(1 << 24);
io.print((1 << 24) >> 24);
io.print(((1 << 24) - 1) & 255);
io.print(((1 << 24) - 1) & ((1 << 16) - 1));
io.print(^9);
io.print(^(1 << 8) + 1);
io.print(10 ^^ 10);
io.print(1 ^^ 2);
io.print(255 | (255 << 8));
io.print(1 | 2);