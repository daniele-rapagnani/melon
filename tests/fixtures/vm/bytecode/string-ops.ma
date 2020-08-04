
.data
    hello "Hello"
    space " "
    world "World"
    excl  "!"

.text
    PUSHK hello
    PUSHK space
    CONCAT
    PUSHK world
    CONCAT
    PUSHK excl
    CONCAT
    DUMPINFO 0