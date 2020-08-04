@function loop 1 0
    .text
        PUSHLOC 0
        PUSHI 0
        EQ
        TESTFALSE 1
        JMP return

        NEWCL loop
        PUSHLOC 0
        PUSHI 1
        SUB

        CALLTAIL 1 1
    
    :return
        PUSHLOC 0
        RETURN 1
@end

.text
    NEWCL loop
    PUSHI 3
    CALL 1 1
    DUMPINFO 0