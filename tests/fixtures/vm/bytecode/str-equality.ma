.data
    str1    "This is a string"
    str2    "This is another string"

    ok      "Equal"
    nok     "Not equal"

.text
    PUSHK str1
    PUSHK str2
    EQ
    TESTTRUE 1
    JMP else
    PUSHK ok
    JMP continue
:else
    PUSHK nok
:continue
    DUMPINFO 0
    HALT
