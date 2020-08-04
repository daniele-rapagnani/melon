
#local a = {}      -- an empty array
#local x = 10
#for i = 1, 2 do
#local j = i
  #a[i] = function () return x + j end
#end
#x = 20

@function adderFactory 1 1
    +upvalue stack 1

    @function adder 0 0
        +upvalue stack 1 # j
        +upvalue parent 0 # x

        .text
            PUSHUPVAL 1
            PUSHUPVAL 0
            ADD

            RETURN 1
    @end

    .text
        PUSHLOC 0
        STORELOC 1

        NEWCL adder
        RETURN 1
@end

@main 3
    .text
        NEWARR
        STORELOC 2 # Arr

        PUSHI 10
        STORELOC 1 # x

        PUSHI 0
        STORELOC 0 # i

    :loop
        PUSHLOC 0
        PUSHI 10
        LT
        TESTTRUE 1
        JMP break

        PUSHLOC 2
        NEWCL adderFactory
        PUSHLOC 0
        CALL 1 1
        PUSHARR

        PUSHLOC 0
        PUSHI 1
        ADD
        STORELOC 0
        JMP loop
    :break

        PUSHI 0
        STORELOC 0

        PUSHI 20
        STORELOC 1

    :loop2
        PUSHLOC 0
        PUSHI 10
        LT
        TESTTRUE 1
        JMP break2

        PUSHLOC 2
        PUSHLOC 0
        GETARR
        CALL 0 1

        PUSHLOC 0
        PUSHI 1
        ADD
        STORELOC 0
        JMP loop2
    :break2

        DUMPINFO 0


