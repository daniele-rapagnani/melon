@function createMatrix 1 4
    .text
        NEWARR
        STORELOC 4 # Array

        PUSH 4
        STORELOC 3 # size

        PUSHI 0
        STORELOC 2 # x

        PUSHI 0
        STORELOC 1 # y

    :yloop
        PUSHLOC 1
        PUSHLOC 3
        LT
        TESTTRUE 1
        JMP ybreak

        PUSHLOC 4 # Array
        NEWARR
        PUSHARR

        PUSHI 0 # x = 0
        STORELOC 2

    :xloop
        PUSHLOC 2
        PUSHLOC 3
        LT
        TESTTRUE 1
        JMP xbreak

        PUSHLOC 4 # Array
        PUSHLOC 1 # y
        GETARR

        PUSHI 0
        PUSHARR

        PUSHLOC 2
        PUSHI 1
        ADD
        STORELOC 2

        JMP xloop
    :xbreak

        PUSHLOC 1
        PUSHI 1
        ADD
        STORELOC 1

        JMP yloop
    :ybreak

    PUSHLOC 4
    RETURN 1
@end

@function getMatrix 3 3
    .text
        PUSH 5
        STORELOC 5 # Array
        
        PUSH 4
        STORELOC 4 # x

        PUSH 3
        STORELOC 3 # y

        PUSHLOC 5
        PUSHLOC 3
        GETARR

        PUSHLOC 4
        GETARR

        RETURN 1
@end

@function setMatrix 4 4
    .text
        PUSH 7
        STORELOC 6 # Array
        
        PUSH 6
        STORELOC 5 # x

        PUSH 5
        STORELOC 4 # y

        PUSH 4
        STORELOC 3 # value

        PUSHLOC 6
        PUSHLOC 4
        GETARR

        PUSHLOC 5
        PUSHLOC 3
        SETARR

        RETURN 0
@end

@main 1
    .text
        NEWCL createMatrix
        PUSHI 4
        CALL 1 1
        STORELOC 0

        PUSHLOC 0
        SIZEARR

        PUSHLOC 0
        GETIARR 1
        SIZEARR

        NEWCL setMatrix
        PUSHLOC 0
        PUSHI 1
        PUSHI 1
        PUSHI 100
        CALL 4 0

        NEWCL getMatrix
        PUSHLOC 0
        PUSHI 1 
        PUSHI 1
        CALL 3 1

        DUMPINFO 0