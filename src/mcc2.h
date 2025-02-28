
#pragma once

// include libraries
#ifdef MCC

#include "mcc2_lib.h"

#else
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <bits/getopt_core.h>
#endif

typedef struct Token Token;
typedef struct Node Node;
typedef struct Parameter Parameter;
typedef struct Relocation Relocation;
typedef struct Ident Ident;
typedef struct Stmt Stmt;
typedef struct IR IR;
typedef struct Reg Reg;
typedef struct RealReg RealReg;
typedef struct Scope Scope;
typedef struct SimpleType SimpleType;
typedef struct QualType QualType;
typedef struct Member Member;
typedef struct Label Label;
typedef struct StringLiteral StringLiteral;
typedef struct SrcFile SrcFile;
typedef struct IncludePath IncludePath;
typedef struct Macro Macro;
typedef struct Warning Warning;
typedef struct IF_GROUP IF_GROUP;
typedef enum TypeKind TypeKind;

extern FILE* fp;
extern char* builtin_def;
extern SrcFile* main_file;

struct IncludePath {
    char* path;
    IncludePath* next;
};

struct SrcFile{
    char*   path;
    char*   name;
    char*   body;
    int     label;
    bool    labeled;
};

struct IF_GROUP {
    bool        cond;
    Token*      head;
    Token*      tail;
    IF_GROUP*   next;
};

typedef enum TokenKind {
    TK_NUM,                     // 数値
    TK_IDENT,
    TK_STRING_LITERAL,          // 文字列リテラル
    TK_PLUS,                    // + 記号
    TK_PLUS_PLUS,               // ++
    TK_MINUS,                   // - 記号
    TK_MINUS_MINUS,             // --
    TK_MUL,                     // *
    TK_DIV,                     // /
    TK_NOT,                     // !
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
    TK_ARROW,                   // ->
    TK_SEMICORON,               // ;
    TK_DOT,                     // .
    TK_DOT_DOT_DOT,             // ...
    TK_COLON,                   // :
    TK_COMMA,                   // ,
    TK_RETURN,                  // return
    TK_BREAK,                   // break
    TK_CONTINUE,                // continue
    TK_SIZEOF,                  // sizeof
    TK_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_GOTO,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
    TK_DO,                      // "do"
    TK_INT,                     // "int"
    TK_CHAR,                    // "char"
    TK_SHORT,                   // "short"
    TK_LONG,                    // "long"
    TK_VOID,                    // "void"
    TK_BOOL,                    // "bool"
    TK_STRUCT,                  // "struct"
    TK_ENUM,                    // "enum"
    TK_UNION,                   // "union"
    TK_CONST,                   // const
    TK_RESTRICT,                // restrict
    TK_VOLATILE,                // volatile
    TK_UNSIGNED,                // unsigned
    TK_SIGNED,                  // signed
    TK_TYPEDEF,                 // typedef
    TK_EXTERN,                  // extern
    TK_STATIC,                  // static
    TK_AUTO,                    // auto
    TK_REGISTER,                // register
    TK_NEWLINE,                 // "\n"

    // builtin
    TK_VA_START,                // va_start
    TK_VA_END,                  // va_end
    TK_VA_ARG,                  // va_arg
    TK_VA_LIST,                 // va_list

    // preprocess
    TK_INCLUDE,                 // #include
    TK_DEFINE,                  // #define
    TK_UNDEF,                   // #undef
    TK_PRAGMA,                  // #pragma
    TK_ONCE,                    // #pragma once
    TK_PP_IF,
    TK_PP_IFDEF,
    TK_PP_IFNDEF,
    TK_PP_ELIF,
    TK_PP_ELSE,
    TK_PP_ENDIF,
    TK_DEFINED,                 // defined
    TK_HASH,
    TK_HASH_HASH,
    TK_SPACE,                   // 空白
    TK_PLACE_HOLDER,            // 空マクロの置き換え
    TK_EOF                      // 終端記号
} TokenKind;

struct Token {
    TokenKind       kind;   // トークンの種類
    char*           pos;    // 位置
    char*           str;    // const stringの中身
    SrcFile*        file;   // ファイル
    unsigned long   val;
    int             len;
    long            row;
    long            col;
    Token*          next;   // 次のトークン
};

// ビルドイントークン定義マクロ
//  sizeof(const string liteal) は終端0を含めるため-1している。
#define MAKE_TOKEN(kind, name)    { kind, name, NULL, NULL, 0, sizeof(name) - 1, 0, 0, NULL,}

struct Macro {
    Token*     name;
    Token*     value;
    Token*     params;
    bool       is_func;
    Macro*     next;
};

struct Relocation{
    int size;
    int data;
    char* label;
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
    char* name;
    Token* tok;             // 宣言部分のトークンポインタ
    int offset;             // ローカル変数ののオフセット
    int is_string_literal;  // 文字列リテラルか？のフラグ
    Scope* scope;           // 関数スコープ
    unsigned long val;      // 数値(enum用)

    Relocation* reloc;      // リロケーション情報

    Node* funcbody;         // 関数のbody
    Parameter* params;
    int stack_size;         // 関数で使用するスタックサイズ
    int is_var_params;      // 可変長引数受け取るか？
    int is_extern;          // externか？
    int is_static;          // staticか？
    Ident* va_area;         // 可変長引数のエリア
    int func_id;            // 識別子のID

    IR* ir_cmd;          // 中間命令の先頭

    // ID_LVAR, ID_GVAR, ID_FUNC -> 識別子の型
    // ID_TYPE -> 型名が表す型情報
    QualType*  qtype;
    Ident* next;
};

struct Parameter {
    Ident*      ident;
    Parameter* next;
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
    ND_NOT,
    ND_LOGIC_OR,
    ND_LOGIC_AND,
    ND_BIT_AND,
    ND_BIT_XOR,
    ND_BIT_OR,
    ND_L_BITSHIFT,
    ND_R_BITSHIFT,
    ND_ASSIGN,
    ND_COMMA,
    ND_RETURN,
    ND_IF,
    ND_IF_ELSE,
    ND_WHILE,
    ND_DO_WHILE,
    ND_FOR,
    ND_BLOCK,
    ND_FUNCCALL,
    ND_ADDR,
    ND_DREF,
    ND_VOID_STMT,
    ND_BREAK,
    ND_CONTINUE,
    ND_SWITCH,
    ND_CASE,
    ND_DEFAULT,
    ND_LABEL,
    ND_GOTO,
    ND_MEMBER,
    ND_CAST,
    ND_NOP,
} NodeKind;

struct Node {
    NodeKind        kind;
    Node*           lhs;
    Node*           rhs;
    unsigned long   val;
    Ident*          ident;
    QualType*       qtype;
    Token*          pos;
    Label*          label;

    Node*           cond;
    Node*           then;
    Node*           elif;
    Node*           body;
    Node*           init;
    Node*           incr;

    Node*           next_case;
    Node*           default_label;

    // 関数呼び出しの引数
    Node*       params;

    Node*       next;
};


/*
    仮想レジスタ
        int idx     : 実レジスタ割付番号
                      この仮想レジスタ
        int val     : レジスタが直値を扱う場合の数値
        Ident* ident: レジスタに割り当てられた識別子
                        ex) ラベル、変数、関数名...
*/
typedef enum RegKind {
    REG_REG = 0,    // 普通のレジスタ
    REG_IMM,        // 直値
    REG_VAR,        // 変数
    REG_ADDR,       // アドレスが入っているレジスタ
    REG_STR,        // 文字列
    REG_FNAME,      // 関数名
} RegKind;

struct Reg {
    RegKind kind;
    int idx;
    int spill_idx;
    unsigned long val;
    bool is_unsigned;
    Ident*  ident;
    Reg*    addr;   // REG_ADDRのアドレスを格納しているところ
    int     size;
    char*   str;
    char*   rreg;
    Token*  tok;
};

typedef enum {
    ierr = -1,
    i8 = 0,
    i16,
    i32,
    i64,
    u8,
    u16,
    u32,
    u64
} SIZE_TYPE_ID;

typedef enum IRCmd{
    // ARITHMETIC
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
    IR_ASSIGN,
    IR_FN_CALL,
    IR_REL,
    IR_CAST,
        // cast t, s1, (null)
        //  s1をtの型にキャストして、仮想レジスタtに格納する

    // REGISTER OPERATION
    IR_MOV,
        // mov (null) s1 s2
        //  s2 -> s1
    IR_RELEASE_REG,
    IR_RELEASE_REG_ALL,
        // relese all real register
    IR_LEA,
        // lea (null) s1 s2
        //  write address of s2 to s1
    IR_LOAD,
        // load (null) s1 s2
        //  [s2] -> s1

    IR_COPY,
        // copy (null) s1 s2
        //  s2 -> s1

    // CONTROL
    IR_RET,
        // ret (null) s1 -> return s1
        // ret (null) (null) -> return with void
    IR_JNZ,
        // jnz (null) s1 (imm)
        //   s1 = 0 -> goto imm
    IR_JZ,
        // jz (null) s1 (imm)
        //   s1 = 0 -> goto imm
    IR_JMP,
        // jmp (null) (imm)
        //  jmp to (imm)
    IR_JE,
        // je imm s1 s2
        //  if s1 == s2, then go to imm
    IR_LABEL,
        // label (null) (imm)
        //   .L(imm):

    // MEMORY
    IR_PUSH,
        // push (null) s1
        //  push s1
    IR_POP,
        // pop (null) s1
        //  pop s1

    // DEFINITION
    IR_FN_LABEL,
        // fnlabel (null) (ident) (imm)
        // -> identという名前の関数の、プロローグを生成する。
        //    immは確保するスタックのサイズ
    IR_FN_END_LABEL,
    IR_GVAR_LABEL,
        // gvarlabel (null) (ident) (imm)
        // -> identというグローバル変数を定義する
        //    immグローバル変数のサイズ
    IR_STATIC_GVAR_LABEL,
        // staticgvarlabel (null) (ident) (imm)
        // -> identというファイルスコープの静的変数を定義する
        //    imm静的変数のサイズ
    IR_STORE_ARG_REG,
    IR_LOAD_ARG_REG,
    IR_SET_FLOAT_NUM,
    IR_EXTERN_LABEL,
        // externlabel (null) (ident) (imm)
        // -> identというラベルを外部に公開する
    IR_VA_START,
        // va_start (target) (imm1) (imm2)
        // 可変長引数を受け取る関数のプロローグ
        // targetはva_elemの場所（rbpからのオフセット）
        // imm1は固定引数の数
        // imm2は浮動小数点引数の数

    // DEBUG
    IR_COMMENT,
        // comment (string)
        // stringをコメントとして出力する
        IR_FILE_SECTION,
        // file (null) (string)
        // ファイル名を設定する

} IRCmd;

/*
    IR : レジスタマシンの中間コード
        IRCmd cmd   : 命令の種別
        Reg *t      : ターゲットレジスタ
        Reg* s1     : ソースレジスタ1
        Reg* s2     : ソースレジスタ2

        演算はs1とs2に対して実施して、tに格納する。
        tがNULLの場合は、s1を上書きする。
*/
struct IR {
    IRCmd cmd;
    IR* next;
    Reg*    t;
    Reg*    s1;
    Reg*    s2;
};

struct Label {
    int no;
    Token* tok;
    bool labeld;
    Label* next;
};

// Scope : スコープを表現するもの
//  int level       : スコープのレベル
//  Ident* ident    : スコープ内に存在する識別子（typedef含む)
//  StringLiteral*  : 文字列リテラルのテーブル。グローバルスコープでだけ意味がある。
//  Scope* parent   : 親のスコープ。
// 識別子の検索は、現在のスコープから親のスコープ側に上がっていく
struct Scope {
    int             level;
    Ident*          ident;
    StringLiteral*  string_literal;
    Scope*          parent;
    Label*          label;
    IR*             ir_cmd;
    SimpleType*     type_tag;
};

enum TypeKind{
    TY_INT,
    TY_POINTER,
    TY_ARRAY,
    TY_BOOL,
    TY_VOID,
    TY_FUNC,
    TY_STRUCT,
    TY_UNION,
    TY_ENUM,
};

struct Member {
    Ident* ident;
    Member* next;
};

typedef enum {
    SCK_NONE = 0,
    SCK_TYPEDEF,
    SCK_EXTERN,
    SCK_STATIC,
    SCK_AUTO,
    SCK_REGISTER
} StorageClassKind;

enum {
    K_VOID      = 1 << 0,
    K_BOOL      = 1 << 2,
    K_CHAR      = 1 << 4,
    K_SHORT     = 1 << 6,
    K_INT       = 1 << 8,
    K_LONG      = 1 << 10,      // up to 2 times can.
    K_USER      = 1 << 16,
    K_SIGNED    = 1 << 17,
    K_UNSIGNED  = 1 << 18
};

struct SimpleType {
    TypeKind    kind;
    Token*      name;
    int         size;
    int         is_unsigned;
    int         array_len;
    bool        is_user_def;
    bool        is_imcomplete;
    QualType*   base_type;
    QualType*   ptr_to;
    Member*     member;         // 構造体 or 共用体のメンバー
    SimpleType* next;
};

struct QualType{
    SimpleType* type;
    bool        is_const;
};

struct StringLiteral{
    char*           name;
    Token*          val;
    StringLiteral*  next;
};

struct Warning {
    char*       warn;
    Warning*    next;
};

// ---------- function prototype ----------
// error.c
void error_tok(Token* tok, char* fmt, ...);
void warn_tok(Token* tok, char* fmt, ...);
void error_at_src(char* pos, SrcFile* src, char* fmt, ...);
void warn_at_src(char* pos, SrcFile* src, char* fmt, ...);
void error(char* fmt, ...);
void unreachable();

// file.c
SrcFile* read_file(const char* filename);
char* get_dirname(char* path);
void file_init();
void print(char* fmt, ...);
void open_output_file(char* filename);
void close_output_file();

// gen_ir.c
void gen_ir();

// gen_x86_64.c
extern int debug_regis;
extern int debug_plvar;
extern int debug_exec;
void gen_x86_64_init();
void gen_x86();

// ident.c
Ident* declare_ident(Token* ident, IdentKind kind, QualType* qty);
Ident* make_ident(Token* ident, IdentKind kind, QualType* qty);
void register_ident(Ident* ident);
Ident* register_string_literal(Token* tok);
void register_tag(SimpleType* type);
Ident* find_ident(Token* tok);
Ident* find_typedef(Token* tok);
Label* find_label(Token* tok);
SimpleType* find_tag(Token* tok);
Label* register_label(Token* tok);
void scope_in();
void scope_out();
int get_stack_size();
Scope* get_current_scope();
Scope* get_global_scope();

// parse.c
void parse(Token* tok);

// preprocess.c
Token* preprocess(Token* token);
void add_include_path(char* path);
void add_predefine_macro(char* path);
void init_preprocess();

// semantics.c
void semantics();

// tokenize.c
Token* tokenize(char* path);
bool is_equal_token(Token* lhs, Token* rhs);
char* get_token_string(Token* tok);
char* get_token_string_literal(Token* tok);
Token* next_newline(Token* tok);
Token* next_token(Token* tok);
Token* skip_to_next(Token* tok, TokenKind kind);
Token* copy_token(Token* tok);
Token* copy_token_list(Token* tok);
Token* copy_token_eol(Token* tok);
Token* get_tokens_tail(Token* tok);
Token* get_token_before_eof(Token* tok);
void output_token(Token* tok);
Token* tokenize_string(char* src);
Token* scan(char* src);

// type.c
extern SimpleType* ty_void;
extern SimpleType* ty_bool;
extern SimpleType* ty_int;
extern SimpleType* ty_char;
extern SimpleType* ty_short;
extern SimpleType* ty_long;
extern SimpleType* ty_uchar;
extern SimpleType* ty_ushort;
extern SimpleType* ty_uint;
extern SimpleType* ty_ulong;

void ty_init();
QualType* pointer_to(QualType* base);
QualType* array_of(QualType* base, int len);
void add_type(Node* node);
bool equal_type(QualType* qty1, QualType* qty2);
SimpleType* new_type(TypeKind kind, int size);
Ident* get_member(SimpleType* type, Token* tok);
void register_typedef(Ident* ident, QualType* qty);
bool is_integer_type(QualType* qty);

QualType* make_qual_type(SimpleType* type);
int get_qtype_size(QualType* qty);
TypeKind get_qtype_kind(QualType* qty);
QualType* get_qtype_ptr_to(QualType* qty);
int get_qtype_is_unsigned(QualType* qty);
int get_qtype_array_len(QualType* qty);

// utility.c
char* strnewcpyn(char* src, int n);
char* format_string(const char* format, ...);
void printline(Token* loc);
