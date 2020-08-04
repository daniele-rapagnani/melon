@function fib 3 1
    .text
        PUSHI 0
        PUSHLOC 0
        EQ
        TESTFALSE 1
        JMP return_a

        PUSHI 1
        PUSHLOC 0
        EQ
        TESTFALSE 1
        JMP return_b

        STORETOPCL 3
        PUSHLOC 3

        PUSHLOC 0 # n - 1
        PUSHI 1
        SUB

        PUSHLOC 2 # b

        PUSHLOC 1 # a + b
        PUSHLOC 2
        ADD

        CALLTAIL 3 1
        
    :return_a
        PUSHLOC 1
        RETURN 1

    :return_b
        PUSHLOC 2
        RETURN 1
@end

.text
    NEWCL fib
    PUSHI 20
    PUSHI 0
    PUSHI 1
    CALL 3 1
    DUMPINFO 0