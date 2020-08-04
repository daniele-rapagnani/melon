@function testFn 0 2
    .data
        a    "This is a test string"
        b    1000

    .text
        PUSHK a
        STORELOC 0
        PUSHK b
        STORELOC 1
        PUSHLOC 0
        PUSHLOC 0
        PUSHLOC 0
        PUSHLOC 1
        PUSHLOC 1
        PUSHLOC 1
        RETURN 6    
@end

.text
    NEWCL testFn
    CALL 0 1
    DUMPINFO 0