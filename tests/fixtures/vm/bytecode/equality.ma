.data
    a 5
    b 6
    c 10.5
    d 12.5

.text
    PUSHK a
    PUSHK a
    EQ
    PUSHK a
    PUSHK b
    EQ
    PUSHK c
    PUSHK c
    EQ
    PUSHK d
    PUSHK c
    EQ
    DUMPINFO 0