#pragma once

typedef struct Token Token;
typedef struct Node Node;
typedef struct Stmt Stmt;
typedef struct Function Function;
typedef struct IR IR;

typedef enum TokenKind {
    TK_NUM,                     // 数値
    TK_ADD,                     // + 記号
    TK_SUB,                     // - 記号
    TK_MUL,
    TK_DIV,
    TK_L_PAREN,                 // (
    TK_R_PAREN,                 // )
    TK_EQUAL,                   // ==
    TK_NOT_EQUAL,               // !=
    TK_L_ANGLE_BRACKET,         // <
    TK_L_ANGLE_BRACKET_EQUAL,   // <=
    TK_R_ANGLE_BRACKET,         // >
    TK_R_ANGLE_BRACKET_EQUAL,   // >=
    TK_SEMICORON,               // ;
    TK_EOF                      // 終端記号
} TokenKind;

struct Token {
    TokenKind   kind;   // トークンの種類
    char*       pos;    // 位置
    int         val;
    Token*      next;   // 次のトークン
};

typedef enum NodeKind {
    ND_NUM,
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_EQUAL,
    ND_NOT_EQUAL,
    ND_LT,
    ND_LE,
} NodeKind;

struct Node {
    NodeKind    kind;
    Node*       lhs;
    Node*       rhs;
    int         val;

    Node*       next;
};


struct Function {
    Node*       stmts;
};

typedef enum IRKind{
    IR_NUM,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_EQUAL,
    IR_NOT_EQUAL,
    IR_LT,
    IR_LE,
    IR_POP,
} IRKind;

struct IR{
    IRKind kind;
    IR* next;
    int val;
};
