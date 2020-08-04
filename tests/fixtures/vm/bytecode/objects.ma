
@function testFn 0 2
    .data
        field1 "field1"
        field2 "field2"
        value1 "value"
        value2 5.0
        value3 1000

    .text
        NEWOBJ
        STORELOC 0

        NEWOBJ
        STORELOC 1

        PUSHLOC 0
        PUSHK field1
        PUSHK value1
        SETOBJ

        PUSHLOC 0
        PUSHK field1
        GETOBJ 0

        PUSHLOC 0
        PUSHK field2
        PUSHK value2
        SETOBJ

        PUSHLOC 0
        PUSHK field2
        GETOBJ 0

        PUSHLOC 0
        PUSHK field1
        GETOBJ 0

        PUSHLOC 1
        PUSHK field1
        PUSHK value2
        SETOBJ

        PUSHLOC 1
        PUSHK field1
        GETOBJ 0

        PUSHLOC 0
        PUSHK field1
        GETOBJ 0

        RETURN 5
@end

.text
    NEWCL testFn
    CALL 0 1
    DUMPINFO 0