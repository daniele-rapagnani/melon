@function fib 1 3
    .text
        PUSHLOC 0
        PUSHI 1
        LTE
        TESTFALSE 1
        JMP base_case

        STORETOPCL 3
        PUSHLOC 3
        PUSHLOC 0
        PUSHI 1
        SUB
        CALL 1 1
        STORELOC 2
        
        STORETOPCL 3
        PUSHLOC 3
        PUSHLOC 0
        PUSHI 2
        SUB
        CALL 1 1
        STORELOC 1

        PUSHLOC 2
        PUSHLOC 1
        ADD

        RETURN 1

    :base_case
        PUSHI 1
        RETURN 1
@end

.text
    NEWCL fib
    PUSHI 15
    CALL 1 1
    DUMPINFO 0