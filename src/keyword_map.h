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
    {   "int",          TK_INT      },
    {   "sizeof",       TK_SIZEOF   },
};
