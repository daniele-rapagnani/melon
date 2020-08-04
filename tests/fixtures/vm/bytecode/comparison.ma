.data
    a   1
    b   2
    c   -5
    d   0
    e   0.124
    f   -5.1223
    g   1000.42

.text
    PUSHK a
    PUSHK b
    LT
    PUSHK b
    PUSHK a
    GT
    PUSHK c
    PUSHK d
    LTE
    PUSHK e
    PUSHK f
    GT
    PUSHK e
    PUSHK g
    LT
    PUSHK a
    PUSHK a
    LT
    PUSHK a
    PUSHK a
    GT
    PUSHK a
    PUSHK a
    LTE
    PUSHK a
    PUSHK a
    GTE

    DUMPINFO 0