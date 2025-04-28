#include "mcc2.h"

SimpleType* ty_void;
SimpleType* ty_bool;
SimpleType* ty_char;
SimpleType* ty_int;
SimpleType* ty_short;
SimpleType* ty_long;
SimpleType* ty_uchar;
SimpleType* ty_ushort;
SimpleType* ty_uint;
SimpleType* ty_ulong;


void ty_init(){
    ty_void = new_type(TY_VOID, 1);
    ty_void->name = create_token(TK_VOID, "void", 4);

    ty_bool = new_type(TY_BOOL, 1);
    ty_bool->name = create_token(TK_BOOL, "bool", 4);

    ty_char = new_type(TY_INT, 1);
    ty_char->name = create_token(TK_CHAR, "char", 4);

    ty_short = new_type(TY_INT, 2);
    ty_short->name = create_token(TK_SHORT, "short", 5);

    ty_int = new_type(TY_INT, 4);
    ty_int->name = create_token(TK_INT, "int", 3);

    ty_long = new_type(TY_INT, 8);
    ty_long->name = create_token(TK_LONG, "long", 4);

    ty_uchar = new_type(TY_INT, 1);
    ty_uchar->is_unsigned = 1;
    ty_uchar->name = create_token(TK_UNSIGNED, "unsigned char", 13);

    ty_ushort = new_type(TY_INT, 2);
    ty_ushort->is_unsigned = 1;
    ty_ushort->name = create_token(TK_UNSIGNED, "unsigned short", 14);

    ty_uint = new_type(TY_INT, 4);
    ty_uint->is_unsigned = 1;
    ty_uint->name = create_token(TK_UNSIGNED, "unsigned int", 12);

    ty_ulong = new_type(TY_INT, 8);
    ty_ulong->is_unsigned = 1;
    ty_ulong->name = create_token(TK_UNSIGNED, "unsigned long", 13);
}

QualType* pointer_to(QualType* base){
    SimpleType* type = new_type(TY_POINTER, 8);
    type->ptr_to = base;
    type->is_unsigned = 1;
    return make_qual_type(type);
}

QualType* array_of(QualType* base, int len){
    SimpleType* type = new_type(TY_ARRAY, 8);
    type->ptr_to = base;
    type->size = get_qtype_size(base);
    type->array_len = len;
    return make_qual_type(type);
}

SimpleType* new_type(TypeKind kind, int size){
    SimpleType* type = calloc(1, sizeof(SimpleType));
    type->kind = kind;
    type->size = size;
    return type;
}

void register_typedef(Ident* ident, QualType* qty){
    ident->qtype = qty;
    ident->kind = ID_TYPE;
    register_ident(ident);
    return;
}

void add_type(Node* node){
    if(node == NULL){
        return;
    }

    add_type(node->params);

    if(node->qtype){
        return;
    }
    add_type(node->lhs);
    add_type(node->rhs);
    add_type(node->cond);
    add_type(node->then);
    add_type(node->elif);
    add_type(node->body);
    add_type(node->init);
    add_type(node->incr);
    add_type(node->next);



    switch(node->kind){
        case ND_NOT:
            if(get_qtype_kind(node->lhs->qtype) == TY_VOID){
                error_tok(node->pos, "invalid operands of types 'void' to unary 'operator'");
            }
            node->qtype = make_qual_type(ty_int);
            break;
        case ND_LOGIC_OR:           // 論理和（1 or 0）
        case ND_LOGIC_AND:          // 論理積（1 or 0）
            if((get_qtype_kind(node->lhs->qtype) == TY_VOID)
                || (get_qtype_kind(node->rhs->qtype) == TY_VOID)){
                error_tok(node->pos, "invalid operands of types 'void' to binary 'operator'");
            }
        case ND_LT:
        case ND_LE:
        case ND_NUM:
        case ND_EQUAL:              // 等価　（1 or 0）
        case ND_NOT_EQUAL:          // 非等価（1 or 0）
            if(node->qtype && get_qtype_kind(node->qtype) == TY_VOID){
                error_tok(node->pos, "void is not allowed.");
            }
            node->qtype = make_qual_type(ty_int);
            break;
        case ND_ASSIGN:             // 代入
            if(get_qtype_kind(node->lhs->qtype) == TY_VOID){
                error_tok(node->lhs->pos, "variable or field declared void");
            }
        case ND_ADD:                // 足し算
        case ND_SUB:                // 引き算
        case ND_MUL:                // 掛け算
        case ND_DIV:                // 割り算
        case ND_MOD:                // 余り
        case ND_BIT_AND:            // bit論理積
        case ND_BIT_XOR:            // bit排他的論理和
        case ND_BIT_OR:             // bit論理和
        case ND_L_BITSHIFT:         // 左bitシフト
        case ND_R_BITSHIFT:         // 右bitシフト
            if((get_qtype_kind(node->lhs->qtype) == TY_VOID)
                || (get_qtype_kind(node->rhs->qtype) == TY_VOID)){
                error_tok(node->pos, "invalid operands of types 'void' to binary 'operator'");
            }
            node->qtype = node->lhs->qtype;
            break;
        case ND_COND_EXPR:          // 三項演算子
            if((get_qtype_kind(node->cond->qtype) == TY_VOID)){
                error_tok(node->pos, "invalid operands of types 'void' in cond");
            }
            node->qtype = node->lhs->qtype;
            break;
        case ND_ADDR:               // & 演算子
            {
                if(get_qtype_kind(node->lhs->qtype) == TY_VOID){
                    error_tok(node->pos, "void is not allowed.");
                }
                node->qtype = pointer_to(node->lhs->qtype);
                break;
            }
        case ND_DREF:
            if(get_qtype_kind(node->lhs->qtype) == TY_VOID){
                error_tok(node->pos, "illegal dereferencing void.");
            }
            if(get_qtype_ptr_to(node->lhs->qtype)){
                node->qtype = get_qtype_ptr_to(node->lhs->qtype);
            } else {
                node->qtype = make_qual_type(ty_int);
            }
            break;
        case ND_FUNCCALL:           // 関数呼び出し（parseで設定済)
        case ND_VAR:               // 変数
            node->qtype = node->ident->qtype;
            break;
        case ND_COMMA:
            node->qtype = node->lhs->qtype;
            break;

        case ND_VA_START:
            // va_start()は型を持たないが、便宜上void型としておく
            node->qtype = make_qual_type(ty_void);
            break;

        // 文 -> 文は評価しても値を返さない＝型がない
        case ND_RETURN:             // リターン文
        case ND_IF:                 // if文
        case ND_IF_ELSE:            // if-else文
        case ND_WHILE:              // while文
        case ND_FOR:                // for文
        case ND_BLOCK:              // 複合文
            break;
    }
}

bool equal_type(QualType* qty1, QualType* qty2){
    SimpleType* ty1 = qty1->type;
    SimpleType* ty2 = qty2->type;
    if(ty1->size != ty2->size){
        return false;
    }

    if(ty1->array_len != ty2->array_len){
        return false;
    }

    if(ty1->kind != ty2->kind){
        return false;
    }

    if(ty1->is_unsigned != ty2->is_unsigned){
        return false;
    }
    return true;
}

Ident* get_member(SimpleType* type, Token* tok){
    for(Member* member = type->member; member; member =member->next){
        if(is_equal_token(member->ident->tok, tok)){
            return member->ident;
        }
    }
    return NULL;
}

bool is_integer_type(QualType* qty){
    TypeKind kind = get_qtype_kind(qty);

    if(kind == TY_INT || kind == TY_ENUM || kind == TY_BOOL || kind == TY_POINTER){
        return true;
    }
}

// ---------------------------------------
// QualType Helper Function
// ---------------------------------------
QualType* make_qual_type(SimpleType* type){
    QualType* qty = calloc(1, sizeof(QualType));
    qty->type = type;
    return qty;
}

int get_qtype_size(QualType* qty){
    if(qty->type->kind == TY_ARRAY){
        return get_qtype_size(qty->type->ptr_to) * qty->type->array_len;
    }
    return qty->type->size;
}

TypeKind get_qtype_kind(QualType* qty){
    return qty->type->kind;
}

QualType* get_qtype_ptr_to(QualType* qty){
    return qty->type->ptr_to;
}

int get_qtype_is_unsigned(QualType* qty){
    return qty->type->is_unsigned;
}

int get_qtype_array_len(QualType* qty){
    return qty->type->array_len;
}

int get_qtype_align(QualType* qty){
    int align = 0;

    if(qty->type->kind == TY_STRUCT || qty->type->kind == TY_UNION){
        // メンバの中で最大のアライメント幅を種痘
        for(Member* member = qty->type->member; member; member = member->next){
            int size = get_qtype_size(member->ident->qtype);
            align = align < size ? size : align;
        }
        return align;
    }

    if(qty->type->kind == TY_ARRAY){
        return get_qtype_align(qty->type->ptr_to);
    }

    if(qty->type->kind == TY_INT || qty->type->kind == TY_ENUM || qty->type->kind == TY_POINTER || qty->type->kind == TY_BOOL){
        return get_qtype_size(qty);
    }
}

int get_qtype_padding(int offset, QualType* qty){
    int padding = 0;
    int align = get_qtype_align(qty);

    // 1byteならどこにおいてもOK
    if (align <= 1) return 0;

    int mod = offset % align;
    padding = mod ? align - mod : 0;
    return padding;
}

bool is_enum_member(QualType* qty, int data){
    SimpleType* ty = qty->type;

    for(Member* cur = ty->member; cur; cur = cur->next){
        if(cur->ident->val == data){
            return true;
        }
    }
    return false;
}