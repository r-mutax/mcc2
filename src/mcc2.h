#pragma once

typedef struct Token Token;
typedef struct Node Node;
typedef struct IR IR;

typedef enum TokenKind {
    TK_NUM,             // 数値
    TK_ADD,             // + 記号
    TK_SUB,             // - 記号
    TK_MUL,
    TK_DIV,
    TK_EOF              // 終端記号
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
} NodeKind;

struct Node {
    NodeKind    kind;
    Node*       lhs;
    Node*       rhs;
    int         val; 
};

typedef enum IRKind{
    IR_NUM,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
} IRKind;

struct IR{
    IRKind kind;
    IR* next;
    int val;
};
