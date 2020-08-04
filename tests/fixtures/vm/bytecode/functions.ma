@function greet 1 0
    .data
        str1 "Hello, "
        str2 "!"

    .text
        PUSHK str1
        PUSHLOC 0
        CONCAT
        PUSHK str2
        CONCAT
        RETURN 1
@end

@function showResult 1 0
    .data
        str1 "This is the result: "

    .text
        PUSHK str1
        PUSHLOC 0
        CONCAT
        RETURN 1
@end

@function repeatString 2 2
    .text
        # 0: current string
        # 1: loop index
        # 2: how many times
        # 3: string

        PUSHI 0
        STORELOC 2

        PUSHLOC 0
        STORELOC 3
    :loop   
        PUSHLOC 2
        PUSHLOC 1
        LT
        TESTTRUE 1
        JMP break
        
        PUSHLOC 3
        PUSHLOC 0
        CONCAT
        STORELOC 3

        PUSHLOC 2
        PUSHI 1
        ADD

        STORELOC 2
        JMP loop
    :break
        PUSHLOC 3
        RETURN 1
@end

@function variadicFunc 2 2
    .data
        oneArgStr "You provided one arg"
        twoArgsStr "You provided two args"

    .text
        PUSH 2
        STORELOC 3       
        PUSH 3
        STORELOC 2

        PUSHLOC 3
        TESTNULL
        JMP pushTwo
        PUSHK oneArgStr
        JMP finish

    :pushTwo
        PUSHK twoArgsStr
    :finish
        RETURN 1
@end

.data
    strFuffy "Fuffy"
    strWiskers "Wiskers"
    strRolling "Rolling "
    intRepeatCount 9

.text
    NEWCL showResult
    NEWCL greet
    PUSHK strFuffy
    CALL 1 1
    CALL 1 1

    NEWCL showResult
    NEWCL greet
    PUSHK strWiskers
    CALL 1 1
    CALL 1 1

    NEWCL repeatString
    PUSHK strRolling
    PUSHK intRepeatCount
    CALL 2 1

    NEWCL variadicFunc
    PUSHI 0
    CALL 1 1

    NEWCL variadicFunc
    PUSHI 0
    PUSHI 0
    CALL 2 1

    DUMPINFO 0