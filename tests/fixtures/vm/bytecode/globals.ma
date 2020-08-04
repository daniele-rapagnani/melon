.data
    key1 "testkey"
    key2 "another key"

    value "a random string value"

.text
    PUSHK key1
    PUSHI 10
    SETGLOBAL

    PUSHK key2
    PUSHK value
    SETGLOBAL

    PUSHK key2
    GETGLOBAL

    PUSHK key1
    GETGLOBAL

    DUMPINFO 0