#include "mcc2.h"
#include <stdlib.h>

Type* ty_int;
Type* ty_char;
Type* new_type(TypeKind kind, int size);

void ty_init(){
    ty_int = new_type(TY_INT, 8);
    ty_char = new_type(TY_INT, 1);
}

Type* pointer_to(Type* base){
    Type* type = new_type(TY_POINTER, 8);
    type->ptr_to = base;
    type->is_unsigned = 1;
    return type;
}

Type* array_of(Type* base, int len){
    Type* type = new_type(TY_ARRAY, 8);
    type->ptr_to = base;
    type->size = base->size;
    type->array_len = len;
    return type;
}

Type* new_type(TypeKind kind, int size){
    Type* type = calloc(1, sizeof(Type));
    type->kind = kind;
    type->size = size;
    return type;
}

void add_type(Node* node){
    if(node == NULL || node->type){
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
    add_type(node->params);
    add_type(node->next);

    switch(node->kind){
        case ND_NUM:
        case ND_EQUAL:              // 等価　（1 or 0）
        case ND_NOT_EQUAL:          // 非等価（1 or 0）
        case ND_LOGIC_OR:           // 論理和（1 or 0）
        case ND_LOGIC_AND:          // 論理積（1 or 0）
            node->type = ty_int;
            break;
        case ND_ADD:                // 足し算
        case ND_SUB:                // 引き算
        case ND_MUL:                // 掛け算
        case ND_DIV:                // 割り算
        case ND_MOD:                // 余り
        case ND_COND_EXPR:          // 三項演算子
        case ND_BIT_AND:            // bit論理積
        case ND_BIT_XOR:            // bit排他的論理和
        case ND_BIT_OR:             // bit論理和
        case ND_L_BITSHIFT:         // 左bitシフト
        case ND_R_BITSHIFT:         // 右bitシフト
        case ND_ASSIGN:             // 代入
            node->type = node->lhs->type;
            break;
        case ND_ADDR:               // & 演算子
            node->type = pointer_to(node->lhs->type);
            break;
        case ND_DREF:
            if(node->lhs->type->ptr_to){
                node->type = node->lhs->type->ptr_to;
            } else {
                node->type = ty_int;
            }
            break;
        case ND_FUNCCALL:           // 関数呼び出し（parseで設定済)
        case ND_VAR:               // 変数
            node->type = node->ident->type;
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