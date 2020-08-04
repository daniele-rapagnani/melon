@function introduceWhiskers 1 0
    .data
        strWhiskersName "Whiskers"

    .text
        PUSHLOC 0
        PUSHK strWhiskersName
        CALL 1 1
        RETURN 1
@end

@function introduceGoodCat 1 0
    .data
        strIntroduction "Here's a good cat: "
    
    .text
        PUSHK strIntroduction
        PUSHLOC 0
        CONCAT
        RETURN 1
@end

@function introduceBadCat 1 0
    .data
        strIntroduction "Here's a bad cat: "
    
    .text
        PUSHK strIntroduction
        PUSHLOC 0
        CONCAT
        RETURN 1
@end

.text
    NEWCL introduceWhiskers
    NEWCL introduceGoodCat
    CALL 1 1

    NEWCL introduceWhiskers
    NEWCL introduceBadCat
    CALL 1 1

    DUMPINFO 0