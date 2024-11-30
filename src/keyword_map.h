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
    {   "long",         TK_LONG     },
    {   "void",         TK_VOID     },
    {   "sizeof",       TK_SIZEOF   },
    {   "break",        TK_BREAK    },
    {   "continue",     TK_CONTINUE },
    {   "switch",       TK_SWITCH   },
    {   "case",         TK_CASE     },
    {   "default",      TK_DEFAULT  },
    {   "goto",         TK_GOTO     },
    {   "struct",       TK_STRUCT   },
    {   "enum",         TK_ENUM     },
    {   "union",        TK_UNION    },
    {   "do",           TK_DO       },
    {   "const",        TK_CONST    },
    {   "restrict",     TK_RESTRICT },
    {   "volatile",     TK_VOLATILE },
    {   "defined",      TK_DEFINED  },
    {   "signed",       TK_SIGNED   },
    {   "unsigned",     TK_UNSIGNED },
    {   "typedef",      TK_TYPEDEF  },
    {   "extern",       TK_EXTERN   },
    {   "static",       TK_STATIC   },
    {   "auto",         TK_AUTO     },
    {   "register",     TK_REGISTER },
};

static KEYWORD_MAP preprocess_keyword_map[] = {
    {   "include",      TK_INCLUDE  },
    {   "define",       TK_DEFINE   },
    {   "undef",        TK_UNDEF    },
    {   "if",           TK_PP_IF       },
    {   "ifdef",        TK_PP_IFDEF    },
    {   "ifndef",       TK_PP_IFNDEF   },
    {   "elif",         TK_PP_ELIF     },
    {   "else",         TK_PP_ELSE     },
    {   "endif",        TK_PP_ENDIF    },
    {   "pragma",       TK_PRAGMA    },
};
