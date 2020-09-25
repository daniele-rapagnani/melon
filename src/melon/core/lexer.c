#include "melon/core/lexer.h"
#include "melon/core/utils.h"
#include "melon/core/memory_utils.h"

#include <stdlib.h>
#include <string.h>

#define melM_error(l, str) ((l)->error ? (l)->error((l)->errorCtx, str, l->file, (l)->line, (l)->column) : 0)

#define melM_isValidNameOpenerChar(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
#define melM_isValidNameChar(c) (melM_isValidNameOpenerChar(c) || (c >= '0' && c <= '9'))

static const char* tokenNames[] = {
    "NONE",
    "LEFT_PAREN",
    "RIGHT_PAREN",
    "LEFT_BRACKET",
    "RIGHT_BRACKET",
    "LEFT_BRACE",
    "RIGHT_BRACE",
    "COLON",
    "SEMICOLON",
    "DOT",
    "COMMA",
    "STAR",
    "SLASH",
    "PERCENT",
    "PLUS",
    "MINUS",
    "PIPE",
    "PIPEPIPE",
    "AMP",
    "AMPAMP",
    "QUESTION",
    "EQ",
    "LT",
    "GT",
    "LTEQ",
    "GTEQ",
    "EQEQ",
    "EQLT",
    "FAT_ARROW",
    "BREAK",
    "ELSE",
    "FALSE",
    "FOR",
    "IF",
    "NULL",
    "RETURN",
    "THIS",
    "TRUE",
    "LET",
    "WHILE",
    "NAME",
    "NUMBER",
    "INTEGER",
    "STRING",
    "EXCLAMATION",
    "HASH",
    "DOTDOT",
    "FUNC",
    "AT",
    "RIGHT_ARROW",
    "DOLLAR",
    "NOTEQ",
    "IN",
    "GTGT",
    "LTLT",
    "CARET",
    "CARETCARET",
    "CONTINUE",
    "QUESTIONQUESTION",
    "ELLIPSIS",
    "EOF"
};

TRet melCreateLexer(Lexer* l, const char* source, TSize len)
{
    return melCreateLexerFile(l, NULL, source, len);
}

TRet melCreateLexerFile(Lexer* l, const char* file, const char* source, TSize len)
{
    l->curChar = l->source = source;
    l->sourceSize = len;
    l->curToken.type = MELON_TOKEN_NONE;
    l->futureToken.type = MELON_TOKEN_NONE;
    l->line = 1;
    l->error = NULL;
    l->errorCtx = NULL;
    l->column = 0;
    l->file = NULL;

    if (melCreateBuffer(&l->curToken.buffer, 0) != 0)
    {
        return 1;
    }

    if (melCreateBuffer(&l->futureToken.buffer, 0) != 0)
    {
        return 1;
    }

    if (file != NULL)
    {
        l->file = (char*)malloc(sizeof(char) * (strlen(file) + 1));
        strcpy(l->file, file);
    }

    return 0;
}

static void melAdvanceLine(Lexer* l)
{
    l->line++;
    l->column = 0;
}

static TRet melAdvanceChar(Lexer* l)
{
    if (((l->curChar + 1) - l->source) >= l->sourceSize)
    {
        l->curChar = NULL;
        return 1;
    }

    if (*l->curChar == '\n')
    {
        melAdvanceLine(l);
    }
    else
    {
        l->column++;
    }

    l->curChar++;
    return 0;
}

static TRet melSkipChars(Lexer* l, TSize count)
{
    if (((l->curChar + count) - l->source) >= l->sourceSize)
    {
        l->curChar = NULL;
        return 1;
    }

    l->column += count;
    l->curChar += count;

    return 0;
}

inline static char melGetChar(Lexer* l)
{
    return l->curChar != NULL ? *l->curChar : 0;
}

static char melPeekCharOffset(Lexer* l, TUint32 offset)
{
    if (((l->curChar + offset) - l->source) >= l->sourceSize)
    {
        return 0;
    }

    return *(l->curChar + offset);
}

static char melPeekChar(Lexer* l)
{
    return melPeekCharOffset(l, 1);
}

static TRet melAdvanceMatchChar(Lexer* l, char c)
{
    if (melPeekChar(l) == c)
    {
        melAdvanceChar(l);
        return 0;
    }

    return 1;
}

static TRet melExpectChar(Lexer* l, char c, const char* error)
{
    if (melGetChar(l) != c)
    {
        melM_error(l, error);
        return 1;
    }

    melAdvanceChar(l);
    return 0;
}

static void setToken(Token* t, MelTokenType type)
{
    t->type = type;
}

#define melM_charIsWhitespace(c) (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\0')

static TRet consumeWhitespace(Lexer* l)
{
    while(melM_charIsWhitespace(melGetChar(l)))
    {
        if (melAdvanceChar(l) != 0)
        {
            return 1;
        }
    }

    return 0;
}

static TRet consumeLineComment(Lexer* l)
{
    if (
        (melGetChar(l) != '/' || melPeekChar(l) != '/')
        && (melGetChar(l) != '#' || melPeekChar(l) != '!')
    )
    {
        return 1;
    }

    melAdvanceChar(l);
    melAdvanceChar(l);

    while(melGetChar(l) != '\0')
    {
        if (melGetChar(l) == '\n')
        {
            // Consume the new line
            melAdvanceChar(l);
            break;
        }

        melAdvanceChar(l);
    }

    return 0;
}

static TRet consumeBlockComment(Lexer* l)
{
    if (melGetChar(l) != '/' || melPeekChar(l) != '*')
    {
        return 1;
    }

    melAdvanceChar(l);
    melAdvanceChar(l);

    while(1)
    {
        if (melGetChar(l) == '\0')
        {
            melM_error(l, "Unterminated block comment");
            break;
        }

        if (melGetChar(l) == '*' && melPeekChar(l) == '/')
        {
            // Consume star and forward slash
            melAdvanceChar(l);
            melAdvanceChar(l);
            break;
        }

        melAdvanceChar(l);
    }

    consumeWhitespace(l);

    return 0;
}

static TUint32 melCheckKeyword(Lexer* l, const char* keyword)
{
    TUint32 keywordLen = strlen(keyword);
    TUint32 lastChar = ((l->curChar + keywordLen) - l->source);

    if (lastChar > l->sourceSize)
    {
        return 0;
    }

    TBool matches = memcmp(keyword, l->curChar, keywordLen) == 0;

    if (!matches)
    {
        return 0;
    }

    // Next char (if there's one) must not be alphanumeric
    if (lastChar + 1 <= l->sourceSize && melM_isValidNameChar(*(l->curChar + keywordLen)))
    {
        return 0;
    }

    return keywordLen;
}

static void melMakeToken(Lexer* l, MelTokenType t, TUint32 len)
{
    l->futureToken.type = t;
    l->futureToken.len = len;
}

static void melMakeTokenAndAdvance(Lexer* l, MelTokenType t, TUint32 len)
{
    melMakeToken(l, t, len);
    melSkipChars(l, len);
}

#define melM_caseOneChar(c, t, l)        \
    case c:                              \
        melMakeTokenAndAdvance(l, t, 1); \
        break

#define melM_caseBranchTwoChars(c1, c2, t1, t2, l)       \
    case c1:                                             \
    {                                                    \
        TBool matched = melAdvanceMatchChar(l, c2) == 0; \
        melMakeTokenAndAdvance(l, matched ? t2 : t1, 1); \
    }                                                    \
    break

#define melM_caseBranchThreeChars(c1, c2, c3, t1, t2, t3, l) \
    case c1:                                                 \
    {                                                        \
        switch (melPeekChar(l))                              \
        {                                                    \
        case c2:                                             \
            melMakeTokenAndAdvance(l, t2, 2);                \
            break;                                           \
        case c3:                                             \
            melMakeTokenAndAdvance(l, t3, 2);                \
            break;                                           \
        default:                                             \
            melMakeTokenAndAdvance(l, t1, 1);                \
            break;                                           \
        }                                                    \
    }                                                        \
    break

#define melM_caseSequenceThreeChars(c1, c2, c3, t1, t2, t3, l) \
    case c1:                                                   \
    {                                                          \
        switch (melPeekChar(l))                                \
        {                                                      \
        case c2:                                               \
        {                                                      \
            switch (melPeekCharOffset(l, 2))                   \
            {                                                  \
            case c3:                                           \
                melMakeTokenAndAdvance(l, t3, 3);              \
                break;                                         \
                                                               \
            default:                                           \
                melMakeTokenAndAdvance(l, t2, 2);              \
                break;                                         \
            }                                                  \
        }                                                      \
        break;                                                 \
        default:                                               \
            melMakeTokenAndAdvance(l, t1, 1);                  \
            break;                                             \
        }                                                      \
    }                                                          \
    break

#define melM_ifKeyword(k, t, l)          \
    if ((s = melCheckKeyword(l, k)) > 0) \
    {                                    \
        melMakeTokenAndAdvance(l, t, s); \
    }

#define melM_ifKeywordElse(k, t, l) \
    melM_ifKeyword(k, t, l) else

const Token* melCurTokenLexer(Lexer* l)
{
    return &l->curToken;
}

static TBool melIsNumber(char c, Lexer* l)
{
    if (c >= '0' && c <= '9')
    {
        return 1;
    }

    if (c == '+' || c == '-')
    {
        char nc = melPeekChar(l);
        return (nc >= '0' && nc <= '9');
    }

    return 0;
}

static TRet melParseNumber(Lexer* l, TUint32* s)
{
    TBool dot = 0;
    TBool exp = 0;
    TBool prevExp = 0;
    *s = 0;

    do
    {
        char c = melGetChar(l);
        char nc = melPeekChar(l);
        char nnc = melPeekCharOffset(l, 2);

        if (c != '-' && c != '+' && c != '.' && c != 'E' && c != 'e' && (c < '0' || c > '9'))
        {
            break;
        }

        if (c == '-' || c == '+')
        {
            if (!prevExp && *s > 1)
            {
                break;
            }

            if (nc < '0' || nc > '9')
            {
                break;
            }
        }
        else if (c == '.')
        {
            if (dot || exp)
            {
                break;
            }

            if (nc < '0' || nc > '9')
            {
                break;
            }

            dot = 1;
        }
        else if (c == 'e' || c == 'E')
        {
            if (exp)
            {
                break;
            }

            if (nc == '+' || nc == '-')
            {
                if (nnc < '0' || nnc > '9')
                {
                    break;
                }
            }
            else if (nc < '0' || nc > '9')
            {
                break;
            }

            exp = prevExp = 1;
        }
        else
        {
            prevExp = 0;
        }

        (*s)++;
    }
    while (melAdvanceChar(l) == 0);

    melMakeToken(l, (dot || exp) ? MELON_TOKEN_NUMBER : MELON_TOKEN_INTEGER, *s);

    return 0;
}

static void melClearBuffer(Lexer* l)
{
    l->futureToken.buffer.size = 0;
}

static void melAppendBuffer(Lexer* l, char c)
{
    if (melWriteBuffer(&l->futureToken.buffer, (const TByte*)&c, 1) != 0)
    {
        melM_error(l, "Can't write to token string buffer");
    }
}

static void melAppendBufferCur(Lexer* l)
{
    melAppendBuffer(l, melGetChar(l));
}

static TRet melUTF8Encode(char *out, uint32_t utf)
{
    if (utf <= 0x7F) 
    {
        // Plain ASCII
        out[0] = (char) utf;
        out[1] = 0;
        return 1;
    }
    else if (utf <= 0x07FF) 
    {
        // 2-byte unicode
        out[0] = (char) (((utf >> 6) & 0x1F) | 0xC0);
        out[1] = (char) (((utf >> 0) & 0x3F) | 0x80);
        out[2] = 0;
        return 2;
    }
    else if (utf <= 0xFFFF) 
    {
        // 3-byte unicode
        out[0] = (char) (((utf >> 12) & 0x0F) | 0xE0);
        out[1] = (char) (((utf >>  6) & 0x3F) | 0x80);
        out[2] = (char) (((utf >>  0) & 0x3F) | 0x80);
        out[3] = 0;
        return 3;
    }
    else if (utf <= 0x10FFFF) 
    {
        // 4-byte unicode
        out[0] = (char) (((utf >> 18) & 0x07) | 0xF0);
        out[1] = (char) (((utf >> 12) & 0x3F) | 0x80);
        out[2] = (char) (((utf >>  6) & 0x3F) | 0x80);
        out[3] = (char) (((utf >>  0) & 0x3F) | 0x80);
        out[4] = 0;
        return 4;
    }
    else 
    { 
        // error - use replacement character
        out[0] = (char) 0xEF;  
        out[1] = (char) 0xBF;
        out[2] = (char) 0xBD;
        out[3] = 0;
        return 0;
    }
}

static void melConvertUnicodeCodepoint(Lexer* l)
{
    melExpectChar(l, 'u', "Invalid unicode code point escape: missing leading 'u'");
    melExpectChar(l, '{', "Invalid unicode code point escape: Missing opening brace.");

    char* start = (char*)l->curChar;
    TSize len = 0;

    while(melGetChar(l) != '}' && melGetChar(l) != '\0')
    {
        len++;
        melAdvanceChar(l);
    }

    melExpectChar(l, '}', "Invalid unicode code point escape: Missing closing brace.");
    
    // Leave the last char to be consumed by the string lexing loop
    l->curChar--;
    char* endChar = (char*)l->curChar;
    *endChar = '\0';

    TUint32 codepoint = strtol(start, NULL, 16);
    *endChar = '}';

    char encodedCP[5];
    TByte cpLen = melUTF8Encode(encodedCP, codepoint);

    for (TByte i = 0; i < cpLen; i++)
    {
        melAppendBuffer(l, encodedCP[i]);
    }
}

static void melConvertEscaped(Lexer* l)
{
    melAdvanceChar(l);

    switch(melGetChar(l))
    {
        case 'a': melAppendBuffer(l, '\a'); break;
        case 'b': melAppendBuffer(l, '\b'); break;
        case 'f': melAppendBuffer(l, '\f'); break;
        case 'n': melAppendBuffer(l, '\n'); break;
        case 'r': melAppendBuffer(l, '\r'); break;
        case 't': melAppendBuffer(l, '\t'); break;
        case 'v': melAppendBuffer(l, '\v'); break;
        
        case 'u': 
            melConvertUnicodeCodepoint(l);
            break;
            
        default:
            melAppendBufferCur(l);
            break;
    }
}

TRet melAdvanceLexer(Lexer* l)
{
    l->curToken = l->futureToken;
    l->futureToken.type = MELON_TOKEN_NONE;

    consumeWhitespace(l);

    TBool shouldReturn = 0;

    if (consumeLineComment(l) == 0 || consumeBlockComment(l) == 0)
    {
        // Preserve the current token
        Token curToken = l->curToken;
        TRet ret = melAdvanceLexer(l);
        l->curToken = curToken;

        return ret;
    }

    l->futureToken.start = l->curChar;
    l->futureToken.line = l->line;
    l->futureToken.column = l->column;

    char c = melGetChar(l);

    switch(c)
    {
        melM_caseOneChar('\0', MELON_TOKEN_EOF, l);
        melM_caseOneChar('(', MELON_TOKEN_LEFT_PAREN, l);
        melM_caseOneChar(')', MELON_TOKEN_RIGHT_PAREN, l);
        melM_caseOneChar('[', MELON_TOKEN_LEFT_BRACKET, l);
        melM_caseOneChar(']', MELON_TOKEN_RIGHT_BRACKET, l);
        melM_caseOneChar('{', MELON_TOKEN_LEFT_BRACE, l);
        melM_caseOneChar('}', MELON_TOKEN_RIGHT_BRACE, l);
        melM_caseOneChar(':', MELON_TOKEN_COLON, l);
        melM_caseOneChar(';', MELON_TOKEN_SEMICOLON, l);
        melM_caseSequenceThreeChars('.', '.', '.', MELON_TOKEN_DOT, MELON_TOKEN_DOTDOT, MELON_TOKEN_ELLIPSIS, l);
        melM_caseOneChar(',', MELON_TOKEN_COMMA, l);
        melM_caseOneChar('*', MELON_TOKEN_STAR, l);
        melM_caseOneChar('/', MELON_TOKEN_SLASH, l);
        melM_caseOneChar('%', MELON_TOKEN_PERCENT, l);
        melM_caseOneChar('+', MELON_TOKEN_PLUS, l);
        melM_caseOneChar('#', MELON_TOKEN_HASH, l);
        melM_caseOneChar('@', MELON_TOKEN_AT, l);
        melM_caseOneChar('$', MELON_TOKEN_DOLLAR, l);
        melM_caseBranchTwoChars('^', '^', MELON_TOKEN_CARET, MELON_TOKEN_CARETCARET, l);
        melM_caseBranchTwoChars('!', '=', MELON_TOKEN_EXCLAMATION, MELON_TOKEN_NOTEQ, l);
        melM_caseBranchTwoChars('-', '>', MELON_TOKEN_MINUS, MELON_TOKEN_RIGHT_ARROW, l);
        melM_caseBranchTwoChars('|', '|', MELON_TOKEN_PIPE, MELON_TOKEN_PIPEPIPE, l);
        melM_caseBranchTwoChars('&', '&', MELON_TOKEN_AMP, MELON_TOKEN_AMPAMP, l);
        melM_caseBranchThreeChars('=', '>', '=', MELON_TOKEN_EQ, MELON_TOKEN_FAT_ARROW, MELON_TOKEN_EQEQ, l);
        melM_caseBranchThreeChars('>', '=', '>', MELON_TOKEN_GT, MELON_TOKEN_GTEQ, MELON_TOKEN_GTGT, l);
        melM_caseBranchThreeChars('<', '=', '<', MELON_TOKEN_LT, MELON_TOKEN_LTEQ, MELON_TOKEN_LTLT, l);
        melM_caseBranchTwoChars('?', '?', MELON_TOKEN_QUESTION, MELON_TOKEN_QUESTIONQUESTION, l);

        case '"':
        {
            TUint32 s = 0;
            char c;
            TBool parsingString = 1;

            // Comsumes the opening quote
            melAdvanceChar(l);
            melClearBuffer(l);

            while(melGetChar(l) != '"')
            {
                switch(melGetChar(l))
                {
                    case '\\':
                        melConvertEscaped(l);
                        break;
                    
                    default:
                        melAppendBufferCur(l);
                        break;
                }
 
                if (melAdvanceChar(l) != 0)
                {
                    melM_error(l, "Unclosed string literal");
                    return 1;
                }

                s++;
            }

            melAppendBuffer(l, '\0');

            melMakeToken(l, MELON_TOKEN_STRING, s);
            l->futureToken.start++; // Removes the starting quote;

            // Skips the ending quote
            melAdvanceChar(l);

            break;
        }

        default:
        {
            // Used by the macro below
            TUint32 s = 0;

            melM_ifKeywordElse("break", MELON_TOKEN_BREAK, l)
            melM_ifKeywordElse("else", MELON_TOKEN_ELSE, l)
            melM_ifKeywordElse("false", MELON_TOKEN_FALSE, l)
            melM_ifKeywordElse("if", MELON_TOKEN_IF, l)
            melM_ifKeywordElse("null", MELON_TOKEN_NULL, l)
            melM_ifKeywordElse("return", MELON_TOKEN_RETURN, l)
            melM_ifKeywordElse("this", MELON_TOKEN_THIS, l)
            melM_ifKeywordElse("true", MELON_TOKEN_TRUE, l)
            melM_ifKeywordElse("let", MELON_TOKEN_LET, l)
            melM_ifKeywordElse("func", MELON_TOKEN_FUNC, l)
            melM_ifKeywordElse("for", MELON_TOKEN_FOR, l)
            melM_ifKeywordElse("in", MELON_TOKEN_IN, l)
            melM_ifKeywordElse("continue", MELON_TOKEN_CONTINUE, l)
            melM_ifKeyword("while", MELON_TOKEN_WHILE, l)
            else if (melIsNumber(c, l))
            {
                melParseNumber(l, &s);
            }
            else if (melM_isValidNameOpenerChar(c))
            {
                s = 1;

                while(melAdvanceChar(l) == 0 && melM_isValidNameChar(melGetChar(l)))
                {
                    s++;
                }

                melMakeToken(l, MELON_TOKEN_NAME, s);
            }
            else
            {
                struct StrFormat sf;
                memset(&sf, 0, sizeof(struct StrFormat));
                melStringFmtUtils(&sf, "Unrecognized token: '%c'\0", melGetChar(l));
                melM_error(l, sf.buffer);
                melStringFmtFreeUtils(&sf);

                return 1;
            }
        }
    }

    return 0;
}

static void melPrintError(Lexer* l, MelTokenType expected, MelTokenType actual)
{
    struct StrFormat sf;
    memset(&sf, 0, sizeof(struct StrFormat));
    melStringFmtUtils(&sf, "Expected token '%s' but found '%s'\0", tokenNames[expected], tokenNames[actual]);
    melM_error(l, sf.buffer);
    melStringFmtFreeUtils(&sf);
}

const Token* melPeekLexer(Lexer* l)
{
    return &l->futureToken;
}

TRet melPopLexer(Lexer* l, Token* outToken)
{
    const Token* t = melCurTokenLexer(l);

    if (t == NULL)
    {
        return 1;
    }

    *outToken = *t;
    melAdvanceLexer(l);

    return 0;
}

TRet melAdvanceIfTypeLexer(Lexer* l, MelTokenType t)
{
    if (melCurTokenLexer(l)->type != t)
    {
        return 1;
    }

    return melAdvanceLexer(l);
}

TRet melPopTypeLexer(Lexer* l, MelTokenType t)
{
    if (melAdvanceIfTypeLexer(l, t) != 0)
    {
        melPrintError(l, t, l->curToken.type);
        return 1;
    }

    return 0;
}

TRet melEnsureTokenLexer(Lexer* l, MelTokenType t)
{
    if (melCheckTokenLexer(l, t) != 0)
    {
        melPrintError(l, t, l->curToken.type);
        return 1;
    }

    return 0;
}

TRet melCheckTokenLexer(Lexer* l, MelTokenType t)
{
    if (melCurTokenLexer(l) == NULL)
    {
        return 1;
    }

    return melCurTokenLexer(l)->type == t ? 0 : 1;
}

TRet melCheckPeekedTokenLexer(Lexer* l, MelTokenType t)
{
    if (melPeekLexer(l) == NULL)
    {
        return 1;
    }

    return melPeekLexer(l)->type == t ? 0 : 1;
}