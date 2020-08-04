
@function funcA 2 1
    @function funcAA 0 1
        +upvalue stack 2 #funcAB
        +upvalue stack 1 #funcA's first param
        +upvalue stack 0 #funcA's second param

        @function funcAAA 0 0
            +upvalue parent 2
            +upvalue parent 1

            .text
                PUSHUPVAL 0
                PUSHUPVAL 1
                MUL
                RETURN 1
        @end

        .text
            #funcA(a, b)

            PUSHUPVAL 0 # funcAB
            PUSHUPVAL 2 # b
            CALL 1 1

            PUSHUPVAL 1 # a
            ADD
            STORELOC 0
            NEWCL funcAAA
            CALL 0 1
            PUSHLOC 0
            ADD

            RETURN 1
    @end

    @function funcAB 1 0
        +upvalue stack 1 #funcA's first param

        .text
            PUSHLOC 0 #funcA's second param (b) and funcAB's first
            PUSHUPVAL 0 #funcA's first param (a)
            ADD
            RETURN 1
    @end

    .text
        NEWCL funcAB # SHOULD BE CLOSED
        STORELOC 2
        NEWCL funcAA

        RETURN 1
@end

@main 1
    .text
        NEWCL funcA
        STORELOC 0

        # ((10 + 5) + 5) + (10 * 5) = 70
        PUSHLOC 0
        PUSHI 10
        PUSHI 5
        CALL 2 1 # funcA returns funcAA bound to funcAB
        CALL 0 1 # calls funcAA(funcAB, 5, 10)

        ## ((10 + 20) + 10) + (10 * 20) = 240
        PUSHLOC 0
        PUSHI 20
        PUSHI 10
        CALL 2 1
        CALL 0 1

        # Just to get consistent output for
        # the tests
        PUSHNULL
        STORELOC 0

        DUMPINFO 0