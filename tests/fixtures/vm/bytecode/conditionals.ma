
.data
    a "Right"
    b "Wrong"
    c "End"

.text
    PUSHTRUE
    TESTTRUE 1
    JMP else
    PUSHK a
    JMP continue
:else
    PUSHK b
:continue
    PUSHK c
    DUMPINFO 0