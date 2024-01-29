#pragma once

typedef struct Token Token;
typedef struct Node Node;
typedef struct Ident Ident;
typedef struct Stmt Stmt;
typedef struct Function Function;
typedef struct IR IR;
typedef struct Parameter Parameter;
typedef struct Scope Scope;
typedef struct Type Type;
typedef enum TypeKind TypeKind;

extern Type* ty_int;

typedef enum TokenKind {
    TK_NUM,                     // 数値
    TK_IDENT,
    TK_PLUS,                    // + 記号
    TK_MINUS,                   // - 記号
    TK_MUL,                     // *
    TK_DIV,                     // /
    TK_PERCENT,                 // %
    TK_PERCENT_EQUAL,           // %=
    TK_PLUS_EQUAL,              // +=
    TK_MINUS_EQUAL,             // -=
    TK_MUL_EQUAL,               // *=
    TK_DIV_EQUAL,               // /=
    TK_QUESTION,                // ?
    TK_ASSIGN,                  // =
    TK_L_PAREN,                 // (
    TK_R_PAREN,                 // )
    TK_L_BRACKET,               // {
    TK_R_BRACKET,               // }
    TK_L_SQUARE_BRACKET,        // [
    TK_R_SQUARE_BRACKET,        // ]
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
    TK_L_BITSHIFT,              // <<
    TK_L_BITSHIFT_EQUAL,        // <<=
    TK_R_BITSHIFT,              // >>
    TK_R_BITSHIFT_EQUAL,        // >>=
    TK_SEMICORON,               // ;
    TK_CORON,                   // :
    TK_CANMA,                   // ,
    TK_RETURN,                  // return
    TK_SIZEOF,                  // sizeof
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_INT,                     // "int"
    TK_CHAR,                    // "char"
    TK_SHORT,                   // "short"
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
    ID_TYPE,            // typedef
    ID_ENUM,
    ID_EOI,             // End of IdentList
} IdentKind;

struct Ident {
    IdentKind   kind;
    Token* tok;         // 宣言部分のトークンポインタ
    int offset;         // ローカル変数ののオフセット

    // ID_LVAR, ID_GVAR, ID_FUNC -> 識別子の型
    // ID_TYPE -> 型名が表す型情報
    Type*   type;
    Ident* next;
};

typedef enum NodeKind {
    ND_NUM,
    ND_VAR,
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_MOD,
    ND_EQUAL,
    ND_NOT_EQUAL,
    ND_LT,
    ND_LE,
    ND_COND_EXPR,
    ND_LOGIC_OR,
    ND_LOGIC_AND,
    ND_BIT_AND,
    ND_BIT_XOR,
    ND_BIT_OR,
    ND_L_BITSHIFT,
    ND_R_BITSHIFT,
    ND_ASSIGN,
    ND_RETURN,
    ND_IF,
    ND_IF_ELSE,
    ND_WHILE,
    ND_FOR,
    ND_BLOCK,
    ND_FUNCCALL,
    ND_ADDR,
    ND_DREF
} NodeKind;

struct Node {
    NodeKind    kind;
    Node*       lhs;
    Node*       rhs;
    int         val;
    Ident*      ident;
    Type*       type;

    Node*       cond;
    Node*       then;
    Node*       elif;
    Node*       body;
    Node*       init;
    Node*       incr;

    // 関数呼び出しの引数
    Node*       params;

    Node*       next;
};

struct Parameter {
    Ident*      ident;
    Parameter*  next;
};

struct Function {
    Ident*      name;
    Node*       stmts;
    Parameter*  params;
    int         stack_size;
    Function*   next;
};

typedef enum IRKind{
    IR_NUM,
    IR_LVAR,
    IR_GVAR,
    IR_ASSIGN,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_MOD,
    IR_EQUAL,
    IR_NOT_EQUAL,
    IR_LT,
    IR_LE,
    IR_BIT_AND,
    IR_BIT_XOR,
    IR_BIT_OR,
    IR_L_BIT_SHIFT,
    IR_R_BIT_SHIFT,
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
    IR_STORE_ARGREG,
    IR_LOAD_ARGREG,
    IR_DREF,
    IR_GVAR_DEF,
} IRKind;

struct IR{
    IRKind kind;
    IR* next;
    int val;
    int address;
    int size;
    Ident*  name;
};

// Scope : スコープを表現するもの
//  int level       : スコープのレベル
//  Ident* ident    : スコープ内に存在する識別子（typedef含む)
//  Scope* parent   : 親のスコープ。
// 識別子の検索は、現在のスコープから親のスコープ側に上がっていく
struct Scope {
    int         level;
    Ident*      ident;
    Scope*      parent;
};

enum TypeKind{
    TY_INT,
    TY_POINTER,
    TY_ARRAY,
    TY_FUNC,
};

struct Type {
    TypeKind    kind;
    int         size;
    int         is_unsigned;
    int         array_len;
    Type*       ptr_to;
};