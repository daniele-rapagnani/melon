@function myFuncA 0 1
    @function myFuncB 0 0
        +upvalue stack 0

        .text
            PUSHUPVAL 0
            PUSHI 1
            ADD
            STOREUPVAL 0
            RETURN 0
    @end

    @function myFuncC 0 0
        +upvalue stack 0

        .text
            PUSHUPVAL 0
            PUSHI 5
            ADD
            STOREUPVAL 0
            RETURN 0
    @end

    @function myFuncEscape 0 0
        +upvalue stack 0

        .text
            PUSHUPVAL 0
            PUSHI 10
            ADD
            RETURN 1
    @end

    @function myFuncEscape2 0 0
        +upvalue stack 0

        .text
            PUSHUPVAL 0
            PUSHI 20
            ADD
            RETURN 1
    @end

    .text
        PUSHI 10
        STORELOC 0
        NEWCL myFuncB
        CALL 0 1
        NEWCL myFuncC
        CALL 0 1
        NEWCL myFuncEscape
        NEWCL myFuncEscape2
        RETURN 2
@end

@function wrapFunc 1 2
    .text
        PUSHLOC 0 # The closure arg
        CALL 0 1

        STORELOC 1 # myFuncEscape2
        STORELOC 2 # myFuncEscape
        
        PUSHLOC 2 # myFuncEscape
        CALL 0 1

        STORELOC 2 # myFuncEscape's result

        PUSHLOC 1 # myFuncEscape2
        CALL 0 1

        STORELOC 1 # myFuncEscape2's result
        RETURN 0
@end

@main 0
    .text
        NEWCL wrapFunc
        NEWCL myFuncA
        CALL 1 1

        GC
        DUMPINFO 2