
.text
    PUSHTRUE
    PUSHFALSE
    PUSHNULL

    PUSHTRUE
    PUSHFALSE
    AND

    # false

    PUSHTRUE
    PUSHTRUE
    AND

    # true

    PUSHFALSE
    PUSHTRUE
    AND

    # false

    PUSHFALSE
    PUSHFALSE
    AND

    # false

    PUSHTRUE
    PUSHFALSE
    OR

    # true

    PUSHTRUE
    PUSHTRUE
    OR

    # true

    PUSHFALSE
    PUSHTRUE
    OR

    # true

    PUSHFALSE
    PUSHFALSE
    OR

    # false

    PUSHTRUE
    NOT

    # false 

    PUSHFALSE
    NOT

    # true

    DUMPINFO 0