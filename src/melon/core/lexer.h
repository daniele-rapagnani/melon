#ifndef __melon__lexer_h__
#define __melon__lexer_h__

#include "melon/core/types.h"
#include "melon/core/buffer.h"

typedef enum {
    MELON_TOKEN_NONE = 0,
    MELON_TOKEN_LEFT_PAREN,
    MELON_TOKEN_RIGHT_PAREN,
    MELON_TOKEN_LEFT_BRACKET,
    MELON_TOKEN_RIGHT_BRACKET,
    MELON_TOKEN_LEFT_BRACE,
    MELON_TOKEN_RIGHT_BRACE,
    MELON_TOKEN_COLON,
    MELON_TOKEN_SEMICOLON,
    MELON_TOKEN_DOT,
    MELON_TOKEN_COMMA,
    MELON_TOKEN_STAR,
    MELON_TOKEN_SLASH,
    MELON_TOKEN_PERCENT,
    MELON_TOKEN_PLUS,
    MELON_TOKEN_MINUS,
    MELON_TOKEN_PIPE,
    MELON_TOKEN_PIPEPIPE,
    MELON_TOKEN_AMP,
    MELON_TOKEN_AMPAMP,
    MELON_TOKEN_QUESTION,
    MELON_TOKEN_EQ,
    MELON_TOKEN_LT,
    MELON_TOKEN_GT,
    MELON_TOKEN_LTEQ,
    MELON_TOKEN_GTEQ,
    MELON_TOKEN_EQEQ,
    MELON_TOKEN_EQLT,
    MELON_TOKEN_FAT_ARROW,
    MELON_TOKEN_BREAK,
    MELON_TOKEN_ELSE,
    MELON_TOKEN_FALSE,
    MELON_TOKEN_FOR,
    MELON_TOKEN_IF,
    MELON_TOKEN_NULL,
    MELON_TOKEN_RETURN,
    MELON_TOKEN_THIS,
    MELON_TOKEN_TRUE,
    MELON_TOKEN_LET,
    MELON_TOKEN_WHILE,
    MELON_TOKEN_NAME,
    MELON_TOKEN_NUMBER,
    MELON_TOKEN_INTEGER,
    MELON_TOKEN_STRING,
    MELON_TOKEN_EXCLAMATION,
    MELON_TOKEN_HASH,
    MELON_TOKEN_DOTDOT,
    MELON_TOKEN_FUNC,
    MELON_TOKEN_AT,
    MELON_TOKEN_RIGHT_ARROW,
    MELON_TOKEN_DOLLAR,
    MELON_TOKEN_NOTEQ,
    MELON_TOKEN_IN,
    MELON_TOKEN_GTGT,
    MELON_TOKEN_LTLT,
    MELON_TOKEN_CARET,
    MELON_TOKEN_CARETCARET,
    MELON_TOKEN_CONTINUE,
    MELON_TOKEN_QUESTIONQUESTION,
    MELON_TOKEN_ELLIPSIS,
    MELON_TOKEN_EOF
} MelTokenType;

typedef struct Token 
{
    MelTokenType type;
    const char* start;
    Buffer buffer;
    TUint32 len;
    TSize line;
    TSize column;
} Token;

typedef void (*LexerErrorCb)(
    void* ctx, 
    const char* message, 
    const char* file,
    TSize line, 
    TSize column
);

typedef struct Lexer 
{
    Token curToken;
    Token futureToken;

    char* file;
    const char* source;
    TSize sourceSize;

    const char* curChar;

    LexerErrorCb error;
    void* errorCtx;

    TSize line;
    TSize column;
} Lexer;

TRet melCreateLexerFile(Lexer* l, const char* file, const char* source, TSize len);
TRet melCreateLexer(Lexer* l, const char* source, TSize len);
TRet melAdvanceLexer(Lexer* l);
const Token* melCurTokenLexer(Lexer* l);
const Token* melPeekLexer(Lexer* l);
TRet melPopLexer(Lexer* l, Token* outToken);
TRet melPopTypeLexer(Lexer* l, MelTokenType t);
TRet melCheckTokenLexer(Lexer* l, MelTokenType t);
TRet melCheckPeekedTokenLexer(Lexer* l, MelTokenType t);
TRet melEnsureTokenLexer(Lexer* l, MelTokenType t);
TRet melAdvanceIfTypeLexer(Lexer* l, MelTokenType t);

#endif // __melon__lexer_h__