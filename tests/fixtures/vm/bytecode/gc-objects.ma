@function pushObjects 0 3
    .data
        key "key"
        value "This is a new and long value"

    .text
        PUSHI 100
        STORELOC 0
        PUSHI 0
        STORELOC 1

    :loop
        PUSHLOC 1
        PUSHLOC 0
        LT
        TESTTRUE 1
        JMP exit

        NEWOBJ
        STORELOC 2

        PUSHLOC 2
        PUSHK key
        PUSHK value
        SETOBJ

        PUSHLOC 2

        PUSHLOC 1
        PUSHI 1
        ADD
        STORELOC 1
        JMP loop
        
    :exit
        RETURN 0
@end

.text
    NEWCL pushObjects
    CALL 0 1
    GC
    DUMPINFO 2
