
.data
    a 10
    b 5
    c 0.5
    d -2.52
    e 3.5

.text
    PUSHI 10
    PUSHI -5
    ADD
    PUSHK a
    PUSHK b
    ADD
    PUSHK b
    ADD
    PUSHK c
    PUSHK d
    ADD
    PUSHK a
    PUSHK b
    SUB
    PUSHK e
    PUSHK c
    DIV
    PUSHK a
    PUSHK b
    DIV
    PUSHK a
    PUSHK b
    MUL
    DUMPINFO 0