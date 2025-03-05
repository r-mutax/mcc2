#include "mcc2.h"

static Scope global_scope = {};
static Scope* cur_scope = &global_scope;
static Scope* func_scope = NULL;
static int stack_size = 0;

static int string_literal_num = 0;



Ident* declare_ident(Token* tok, IdentKind kind, QualType* qty){
    Ident* ident = calloc(1, sizeof(Ident));

    // サイズを計算
    int size = 0;
    if((ident->kind == ID_LVAR) && (cur_scope->level != 0)){
        size = get_qtype_size(qty);
    }

    // パディングを計算
    int padding = get_qtype_padding(stack_size, qty);

    // スタックサイズを更新
    stack_size += size + padding;

    ident->kind = kind;
    ident->name = strnewcpyn(tok->pos, tok->len);
    ident->tok = tok;
    ident->offset = stack_size;
    ident->qtype = qty;


    ident->next = cur_scope->ident;
    cur_scope->ident = ident;
    return ident;
}

// Identの作成
Ident* make_ident(Token* tok, IdentKind kind, QualType* qty){
    Ident* ident = calloc(1, sizeof(Ident));
    
    ident->kind = kind;
    ident->name = strnewcpyn(tok->pos, tok->len);
    ident->tok = tok;
    ident->offset = 0;
    ident->qtype = qty;
    return ident;
}

void register_ident(Ident* ident){
    QualType* qty = ident->qtype;

    int size = 0;
    if((ident->kind == ID_LVAR) && (cur_scope->level != 0)){
        if(!ident->is_extern){
            size = get_qtype_size(qty);
        }
    }

    // パディングを計算
    int padding = get_qtype_padding(stack_size, qty);

    // スタックサイズを更新
    stack_size += size + padding;

    ident->offset = stack_size;

    ident->next = cur_scope->ident;
    cur_scope->ident = ident;
}

void register_tag(SimpleType* type){
    type->next = cur_scope->type_tag;
    cur_scope->type_tag = type;
}

Ident* register_string_literal(Token* tok){
    StringLiteral* sl = calloc(1, sizeof(StringLiteral));
    sl->name = calloc(1, sizeof(20));
    sprintf(sl->name, ".LSTR%d", string_literal_num++);
    sl->val = tok;

    sl->next = global_scope.string_literal;
    global_scope.string_literal = sl;

    Ident* ident = calloc(1, sizeof(Ident));
    ident->kind = ID_GVAR;
    ident->name = sl->name;
    ident->tok = sl->val;
    ident->is_string_literal = 1;
    ident->qtype = array_of(make_qual_type(ty_char), sl->val->len + 1);

    ident->next = global_scope.ident;
    global_scope.ident = ident;
    return ident;
}

Label* register_label(Token* tok){
    Label* label = calloc(1, sizeof(Label));
    label->tok = tok;
    label->next = func_scope->label;
    func_scope->label = label;
    return label;
}

Label* find_label(Token* tok){
    for(Label* label = func_scope->label; label; label = label->next){
        if(is_equal_token(label->tok, tok)){
            return label;
        }
    }
    return false;
}

Ident* find_ident(Token* tok){
    for(Scope* sc = cur_scope; sc; sc = sc->parent){
        for(Ident* id = sc->ident; id; id = id->next){
            Token* lhs = tok;
            if((lhs->len == strlen(id->name))
                    && (!memcmp(lhs->pos, id->name, lhs->len))){
                return id;
            }
        }
    }

    return NULL;
}

Ident* find_typedef(Token* tok){
    for(Scope* sc = cur_scope; sc; sc = sc->parent){
        for(Ident* id = sc->ident; id; id = id->next){
            if(id->kind == ID_TYPE){
                Token* lhs = tok;
                if((lhs->len == strlen(id->name))
                        && (!memcmp(lhs->pos, id->name, lhs->len))){
                    return id;
                }
            }
        }
    }
    return NULL;
}

SimpleType* find_tag(Token* tok){
    for(Scope* sc = cur_scope; sc; sc = sc->parent){
        for(SimpleType* ty = sc->type_tag; ty; ty = ty->next){
            if(is_equal_token(ty->name, tok)){
                return ty;
            }
        }
    }
    return NULL;
}


int get_stack_size(){
    return stack_size;
}

int set_stack_size(int size){
    stack_size = size;
}

void scope_in(){
    Scope* new_scope = calloc(1, sizeof(Scope));
    new_scope->level = cur_scope->level + 1;
    new_scope->parent = cur_scope;

    if(cur_scope->level == 0){
        func_scope = new_scope;
    }

    cur_scope = new_scope;
}

void scope_out(){
    if(cur_scope->level == 0) return;
    if(cur_scope->level == 1) func_scope = NULL;

    Scope* buf = cur_scope;
    cur_scope = cur_scope->parent;

    if(cur_scope == &global_scope){
        stack_size = 0;
    }
}

Scope* get_current_scope(){
    return cur_scope;
}

Scope* get_global_scope(){
    return &global_scope;
}