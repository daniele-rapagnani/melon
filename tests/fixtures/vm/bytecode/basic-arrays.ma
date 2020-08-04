

@main 1
    .text
        NEWARR
        STORELOC 0

        PUSHLOC 0
        PUSHI 100
        PUSHARR

        PUSHLOC 0
        PUSHI 200
        PUSHARR

        PUSHLOC 0
        PUSHI 300
        PUSHARR

        PUSHLOC 0
        PUSHI 400
        SETIARR 0

        PUSHLOC 0
        GETIARR 0

        PUSHLOC 0
        GETIARR 1

        PUSHLOC 0
        GETIARR 2

        DUMPINFO 0

        POP
        POP
        POP

        PUSHLOC 0
        PUSHI 0
        GETARR

        PUSHLOC 0
        PUSHI 1
        GETARR

        ADD

        PUSHLOC 0
        PUSHI 2
        PUSH 2
        SETARR

        POP

        PUSHLOC 0
        PUSHI 2
        GETARR    

        PUSHLOC 0
        SIZEARR

        PUSHNULL
        STORELOC 0

        DUMPINFO 0
