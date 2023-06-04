
typedef struct Token Token;
typedef struct Node Node;

typedef enum TokenKind {
    TK_NUM,             // 数値
    TK_ADD,             // + 記号
    TK_SUB,             // - 記号
    TK_EOF              // 終端記号
} TokenKind;

struct Token {
    TokenKind   kind;   // トークンの種類
    char*       pos;    // 位置
    Token*      next;   // 次のトークン
};
