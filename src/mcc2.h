#pragma once

typedef struct Token Token;
typedef struct Node Node;
typedef struct Ident Ident;
typedef struct Stmt Stmt;
typedef struct Function Function;
typedef struct IR IR;

typedef enum TokenKind {
    TK_NUM,                     // 数値
    TK_IDENT,
    TK_ADD,                     // + 記号
    TK_SUB,                     // - 記号
    TK_MUL,
    TK_DIV,
    TK_ASSIGN,                  // =
    TK_L_PAREN,                 // (
    TK_R_PAREN,                 // )
    TK_L_BRACKET,               // {
    TK_R_BRACKET,               // }
    TK_AND,                     // &
    TK_AND_AND,                 // &&
    TK_HAT,                     // ^
    TK_PIPE,                    // |
    TK_PIPE_PIPE,               // ||
    TK_EQUAL,                   // ==
    TK_NOT_EQUAL,               // !=
    TK_L_ANGLE_BRACKET,         // <
    TK_L_ANGLE_BRACKET_EQUAL,   // <=
    TK_R_ANGLE_BRACKET,         // >
    TK_R_ANGLE_BRACKET_EQUAL,   // >=
    TK_SEMICORON,               // ;
    TK_RETURN,                  // return
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_EOF                      // 終端記号
} TokenKind;

struct Token {
    TokenKind   kind;   // トークンの種類
    char*       pos;    // 位置
    int         val;
    int         len;
    Token*      next;   // 次のトークン
};

typedef enum IdentKind {
    ID_LVAR,
    ID_GVAR,
    ID_FUNC,
    ID_ENUM,
    ID_EOI,             // End of IdentList
} IdentKind;

struct Ident {
    IdentKind   kind;
    Token* tok;         // 宣言部分のトークンポインタ
    int size;           // ローカル変数のサイズ
    int offset;         // ローカル変数ののオフセット
    int level;          // スコープのレベル
    Ident* next;
};

typedef enum NodeKind {
    ND_NUM,
    ND_LVAR,
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_EQUAL,
    ND_NOT_EQUAL,
    ND_LT,
    ND_LE,
    ND_LOGIC_OR,
    ND_LOGIC_AND,
    ND_BIT_AND,
    ND_BIT_XOR,
    ND_BIT_OR,
    ND_ASSIGN,
    ND_RETURN,
    ND_IF,
    ND_IF_ELSE,
    ND_WHILE,
    ND_FOR,
    ND_BLOCK,
    ND_FUNCCALL,
} NodeKind;

struct Node {
    NodeKind    kind;
    Node*       lhs;
    Node*       rhs;
    int         val;
    Ident*      ident;

    Node*       cond;
    Node*       then;
    Node*       elif;
    Node*       body;
    Node*       init;
    Node*       incr;

    Node*       next;
};


struct Function {
    Ident*      name;
    Node*       stmts;
    int         stack_size;
    Function*   next;
};

typedef enum IRKind{
    IR_NUM,
    IR_LVAR,
    IR_ASSIGN,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_EQUAL,
    IR_NOT_EQUAL,
    IR_LT,
    IR_LE,
    IR_BIT_AND,
    IR_BIT_XOR,
    IR_BIT_OR,
    IR_POP,
    IR_LOAD,
    IR_FN_START,
    IR_FN_END,
    IR_FN_LABEL,
    IR_FN_CALL_NOARGS,
    IR_LABEL,
    IR_JMP,
    IR_JZ,
    IR_JNZ,
} IRKind;

struct IR{
    IRKind kind;
    IR* next;
    int val;
    int address;
    int size;
    Ident*  name;
};
