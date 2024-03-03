#pragma once
#include "mcc2.h"

typedef struct KEYWORD_MAP {
    char*       keyword;
    TokenKind   kind;
} KEYWORD_MAP;

static KEYWORD_MAP keyword_map[] = {
    {   "return",       TK_RETURN   },
    {   "if",           TK_IF       },
    {   "else",         TK_ELSE     },
    {   "while",        TK_WHILE    },
    {   "for",          TK_FOR      },
    {   "char",         TK_CHAR     },
    {   "short",        TK_SHORT    },
    {   "int",          TK_INT      },
    {   "sizeof",       TK_SIZEOF   },
    {   "break",        TK_BREAK    },
    {   "continue",     TK_CONTINUE },
    {   "switch",       TK_SWITCH   },
    {   "case",         TK_CASE     },
    {   "default",      TK_DEFAULT  },
    {   "goto",         TK_GOTO     },
    {   "struct",       TK_STRUCT   },
    {   "union",        TK_UNION    },
    {   "do",           TK_DO       },
};
