#include "mcc2.h"

/*
    program = ( function | declaration )*
    function = ident '(' (declspec ident)*'){' compound_stmt
    stmt = expr ';' |
            'return' expr ';' |
            'if(' expr ')' stmt ('else' stmt)? |
            'while(' expr ')' stmt |
            'for(' (expr | declspec ident ('=' assign) ? )? ';' expr? '; expr? )' stmt |
            '{' compound_stmt |
            'switch(' expr ')' stmt |
            'case' const_expr ':' |
            'default:' |
            'goto' ident ';'
    compound_stmt = stmt* | declaration* '}'
    declaration = declare '=' (expr)? ';'
    declare = declspec ident
    declspec = 'int' '*' * | 'char' '*' * | 'short' '*' * | 'struct' ident | 'union' ident
    expr = assign (',' assign)*
    assign = cond_expr ( '=' assign
                        | '+=' assign
                        | '-=' assign
                        | '*=' assign
                        | '/=' assign
                        | '%=' assign
                        | '<<' assign
                        | '>>' assign
                    )?
    cond_expr = logicOR ( '?' expr : cond_expr );
    logicOr = logicAnd ( '||' logicOr )*
    logicAnd = bitOr ( '&&' bitOr )*
    bitOr = bitXor ('|' bitXor)*
    bitXor = bitAnd ('^' bitAnd )*
    bitAnd = equality ('&' equality )*
    equality = relational ('==' relational | '!=' relational)*
    relational = bitShift ('<' bitShift | '<=' bitShift | '>' bitShift | '>=' bitShift)*
    bitShift = add ('<<' add | '>>' add)?
    add = mul ('+' mul | '-' mul)*
    mul = unary ('*' cast | '/' cast | '%' cast)*
    cast = '(' type-name ')' cast | unary
    unary = ('+' | '-' | '&' unary | '*' unary | 'sizeof' unary )? postfix
    postfix = primary ('[' expr ']')*
    primary = '(' expr ')' | num | ident | ident '()'
*/

typedef struct Initializer Initializer;
struct Initializer {
    QualType* qtype;
    Node* init_node;        // 初期化のノード（ND_ASSIGN, ND_BLOCKなど）
    Initializer* child;     // 子の初期化子
    Initializer* next;
};

static Node* switch_node = NULL;
static QualType* cur_func_type = NULL;
static Token* token = NULL;

static void Program();
static void function(QualType* ty, StorageClassKind sck);
static Node* stmt();
static Node* compound_stmt();
static Node* declaration(QualType* ty, StorageClassKind sck);
static Ident* declare(QualType* ty, StorageClassKind sck);
static QualType* declspec(StorageClassKind* sck);
static QualType* type_suffix(QualType* ty);
static QualType* array_type_suffix(QualType* ty);
static Initializer* initialize(QualType* ty, Node* var_node);
static Relocation* make_relocation(Initializer* init, QualType* qty);
static bool check_storage_class_keyword(StorageClassKind* sck, Token* tok);
static void count_decl_spec(int* type_flg, int flg, Token* tok);
static Member* struct_or_union_member();
static SimpleType* struct_or_union_spec(bool is_union);
static SimpleType* enum_spec();
static Member* enum_member();
static int emit(Node* expr);
static int emit2(Node* expr, char** label);
static Node* expr();
static Node* assign();
static Node* cond_expr();
static Node* logicOr();
static Node* logicAnd();
static Node* bitOr();
static Node* bitXor();
static Node* bitAnd();
static Node* equality();
static Node* relational();
static Node* bitShift();
static Node* add();
static Node* mul();
static Node* cast();
static Node* unary();
static Node* postfix();
static Node* primary();
static Node* const_expr();
static Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
static Node* new_node_add(Node* lhs, Node* rhs);
static Node* new_node_sub(Node* lhs, Node* rhs);
static Node* new_node_div(Node* lhs, Node* rhs);
static Node* new_node_mul(Node* lhs, Node* rhs);
static Node* new_node_mod(Node* lhs, Node* rhs);
static Node* new_node_num(unsigned long num);
static Node* new_node_var(Ident* ident);
static Node* new_node_memzero(Node* var_node);
static Node* new_node_member(Node* var_node, Ident* member);
static Node* new_inc(Node* var);
static Node* new_dec(Node* var);
static bool is_function();
static bool is_var_func();

// ----------------------------------------
// トークン操作
void expect_token(TokenKind kind);
bool consume_token(TokenKind kind);
Token* consume_ident();
Token* expect_ident();
Token* consume_string_literal();
Token* consume_typedef_name();
unsigned long expect_num();
bool is_eof();
bool is_type();
bool is_label();
Token* get_token();
void set_token(Token* tok);

// ビルトインのトークン定義
Token unnamed_struct_token = MAKE_TOKEN(TK_IDENT, "__unnamed_struct");
Token unnamed_enum_token = MAKE_TOKEN(TK_IDENT, "__unnamed_enum");
Token va_arena_token = MAKE_TOKEN(TK_IDENT, "__va_area__");
Token builtin_va_elem_token = MAKE_TOKEN(TK_IDENT, "__builtin_va_elem");
Token spill_area_token = MAKE_TOKEN(TK_IDENT, "__spill_area__");
Token va_elem_gp_offset_token = MAKE_TOKEN(TK_IDENT, "gp_offset");
Token va_elem_fp_offset_token = MAKE_TOKEN(TK_IDENT, "fp_offset");
Token va_elem_overflow_arg_area_token = MAKE_TOKEN(TK_IDENT, "overflow_arg_area");
Token va_elem_reg_save_area_token = MAKE_TOKEN(TK_IDENT, "reg_save_area");
Token tmp_token = MAKE_TOKEN(TK_IDENT, "__tmp__");

#define VA_AREA_SIZE 8 * 6 + 16 * 8

void parse(Token* tok){
    token = tok;
    Program();
    return;
}

void Program(){
    while(!is_eof()){
        StorageClassKind sck = SCK_NONE;
        QualType* qty = declspec(&sck);

        if(consume_token(TK_SEMICORON)){
            continue;
        }

        if(is_function()){
            function(qty, sck);
        } else {
            declaration(qty, sck);
        }
    }

    return;
}

static void function(QualType* func_type, StorageClassKind sck){

    while(consume_token(TK_MUL)){
        func_type = pointer_to(func_type);
    }

    Token* tok = consume_ident();
    if(!tok){
        // 識別子がない場合は、関数宣言がない
        return;
    }

    // 前方宣言があるか検索
    Ident* func = find_ident(tok);
    bool has_forward_def = false;       // 前方宣言あるか？（パラメータ個数チェック用）
    if(!func){
        func = declare_ident(tok, ID_FUNC, func_type);
        func->vars = new_PList();
    } else {
        // ある場合は戻り値型がconflictしてないかチェック
        if(!equal_type(func_type, func->qtype)){
            error_tok(tok, "conflict definition type.");
        }
        has_forward_def = true;
        func->tok = tok;
    }

    // ここでスコープインして、仮引数は関数スコープ内で宣言するため、ローカル変数と同等に扱える。
    scope_in();
    func->is_var_params = is_var_func();
    if(func->is_var_params){
        // 可変長引数関数の場合は、レジスタの退避領域を確保するため、
        // stack_sizeをずらす
        set_stack_size(get_stack_size() + VA_AREA_SIZE);
    }

    expect_token(TK_L_PAREN);

    if(!consume_token(TK_R_PAREN)){
        Parameter head = {};
        Parameter* cur = &head;
        do {
            if(consume_token(TK_DOT_DOT_DOT)){
                func->is_var_params = true;
                break;
            } else {
                StorageClassKind sck = 0;
                QualType* qty = declspec(&sck);
                Ident* ident = declare(qty, sck);
                register_ident(ident);
                Parameter* param = calloc(1, sizeof(Parameter));
                param->ident = ident;
                cur = cur->next = param;
            }
        } while(consume_token(TK_COMMA));
        func->params = head.next;
        expect_token(TK_R_PAREN);
    }

    func->scope = get_current_scope();
    if(sck == SCK_STATIC){
        func->is_static = true;
    }

    if(consume_token(TK_SEMICORON)){
        // ここでセミコロンがあるなら前方宣言
        scope_out();
        return;
    } else {
        if(has_forward_def && func->funcbody){
            // すでに定義された関数のbodyがあるのに、ここでも定義している
            error_tok(tok, "Conflict function definition.");
        }
    }

    // ここからは関数bodyをコンパイルする
    set_current_func(func);

    // 実レジスタ退避用の領域を確保(とりあえず30個確保する)
    Ident* spill_area = make_ident(&spill_area_token, ID_LVAR, make_qual_type(ty_char));
    spill_area->qtype = array_of(make_qual_type(ty_char), 8 * 30);
    spill_area->is_builtin = 1;
    register_ident(spill_area);

    // 関数の戻り値の型を保持しておく
    cur_func_type = func_type;

    // 関数のbodyをパース
    expect_token(TK_L_BRACKET);
    func->funcbody = compound_stmt();
    func->stack_size = get_stack_size();
    scope_out();
    add_type(func->funcbody);

    return;
}

static Node* stmt(){
    Token* tok = get_token();
    if(consume_token(TK_RETURN)){
        Node* node = NULL;

        if(get_qtype_kind(cur_func_type) == TY_VOID){
            if(consume_token(TK_SEMICORON)){
                node = new_node(ND_RETURN, NULL, NULL);
                node->pos = tok;
                return node;
            } else {
                node = expr();
                node = new_node(ND_RETURN, node, NULL);
                node->pos = tok;
                expect_token(TK_SEMICORON);
                return node;
            }
        } else {
            if(consume_token(TK_SEMICORON)){
                error_tok(tok, "return value is not found.");
            }
            node = expr();
            node = new_node(ND_RETURN, node, NULL);
            node->pos = tok;
            expect_token(TK_SEMICORON);
            return node;
        }
    } else if(consume_token(TK_IF)){
        Node* node = new_node(ND_IF, NULL, NULL);
        node->pos = tok;
        expect_token(TK_L_PAREN);
        node->cond = expr();
        expect_token(TK_R_PAREN);
        node->then = stmt();

        // else側がある場合
        if(consume_token(TK_ELSE)){
            node->elif = stmt();
            node->kind = ND_IF_ELSE;
        }
        return node;
    } else if(consume_token(TK_WHILE)){
        Node* node = new_node(ND_WHILE, NULL, NULL);
        node->pos = tok;
        expect_token(TK_L_PAREN);
        node->cond = expr();
        expect_token(TK_R_PAREN);
        node->body = stmt();
        return node;
    } else if(consume_token(TK_DO)){
        Node* node = new_node(ND_DO_WHILE, NULL, NULL);
        node->pos = tok;
        node->body = stmt();
        expect_token(TK_WHILE);
        expect_token(TK_L_PAREN);
        node->cond = expr();
        expect_token(TK_R_PAREN);
        expect_token(TK_SEMICORON);
        return node;
    } else if(consume_token(TK_FOR)){
        Node* node = new_node(ND_FOR, NULL, NULL);
        node->pos = tok;
        expect_token(TK_L_PAREN);

        // 初期化式
        if(consume_token(TK_SEMICORON)){
            node->init = NULL;
        } else {
            if(is_type()){
                StorageClassKind sck = 0;
                QualType* qty = declspec(&sck);
                Ident* ident = declare(qty, sck);
                register_ident(ident);
                if(consume_token(TK_ASSIGN)){
                    Node* var_node = new_node_var(ident);
                    var_node->pos = ident->tok;
                    node->init = new_node(ND_ASSIGN, var_node, assign());
                }
            } else {
                node->init = expr();
            }
            expect_token(TK_SEMICORON);
        }

        // 継続条件の式
        if(consume_token(TK_SEMICORON)){
            node->cond = new_node_num(1);
        } else {
            node->cond = expr();
            expect_token(TK_SEMICORON);
        }

        // イテレートの式
        if(consume_token(TK_R_PAREN)){
            node->incr = NULL;
        } else {
            node->incr = expr();
            expect_token(TK_R_PAREN);
        }
        node->body = stmt();
        return node;
    } else if(consume_token(TK_L_BRACKET)){
        return compound_stmt();
    } else if(consume_token(TK_SEMICORON)){
        Node* node = new_node(ND_VOID_STMT, NULL, NULL);
        node->pos = tok;
        return node;
    } else if(consume_token(TK_BREAK)){
        Node* node = new_node(ND_BREAK, NULL, NULL);
        node->pos = tok;
        return node;
    } else if(consume_token(TK_CONTINUE)){
        Node* node = new_node(ND_CONTINUE, NULL, NULL);
        node->pos = tok;
        return node;
    } else if(consume_token(TK_SWITCH)){
        Node* node = new_node(ND_SWITCH, NULL, NULL);
        Node* switch_back = switch_node;
        switch_node = node;

        node->pos = tok;
        expect_token(TK_L_PAREN);
        node->cond = expr();
        expect_token(TK_R_PAREN);
        node->body = stmt();

        switch_node = switch_back;
        return node;
    } else if(consume_token(TK_CASE)){
        if(switch_node == NULL){
            error_tok(tok, "case is not within a switch statement.");
        }

        Node* node = new_node(ND_CASE, NULL, NULL);
        node->lhs = const_expr(); 
        consume_token(TK_COLON);
        node->pos = tok;

        node->next_case = switch_node->next_case;
        switch_node->next_case = node;
        return node;
    } else if(consume_token(TK_DEFAULT)){
        Node* node = new_node(ND_DEFAULT, NULL, NULL);
        node->pos = tok;
        consume_token(TK_COLON);

        if(switch_node->default_label){
            error_tok(tok, "multiple default label.");
        }
        switch_node->default_label = node;
        return node;
    } else if(is_label()){
        Node* node = new_node(ND_LABEL, NULL, NULL);
        node->pos = tok;
        Token* label_ident = expect_ident();
        expect_token(TK_COLON);

        Label* label = find_label(label_ident);
        if(!label){
            label = register_label(label_ident);
        }
        label->labeld = true;
        node->label = label;
        return  node;
    } else if(consume_token(TK_GOTO)){
        Node* node = new_node(ND_GOTO, NULL, NULL);
        node->pos = tok;
        Token* label_ident = expect_ident();
        Label* label = find_label(label_ident);
        if(!label){
            label = register_label(label_ident);
        }
        node->label = label;
        return  node;
    } else {
        Node* node = expr();
        expect_token(TK_SEMICORON);
        node->pos = tok;
        return node;
    }
}

static Node* compound_stmt(){
    Node* node = new_node(ND_BLOCK, NULL, NULL);

    Node head = {};
    Node* cur = &head;
    while(!consume_token(TK_R_BRACKET)){
        if(is_type()){
            StorageClassKind sck = SCK_NONE;
            QualType* qty = declspec(&sck);

            if(consume_token(TK_SEMICORON)){
                continue;
            }

            Node* node = declaration(qty, sck);
            if(node){
                cur = cur->next = node;
            }
        } else {
            cur->next = stmt();
            cur = cur->next;
        }
        add_type(cur);
    }
    node->body = head.next;
    return node;
}

static Node* declaration(QualType* qty, StorageClassKind sck){
    SimpleType* ty = qty->type;

    if(ty->kind == TY_STRUCT && consume_token(TK_SEMICORON)){
        // 構造体の登録を行う
        return new_node(ND_VOID_STMT, NULL, NULL);
    }

    if(ty->kind == TY_UNION && consume_token(TK_SEMICORON)){
        // 共用体の登録を行う
        return new_node(ND_VOID_STMT, NULL, NULL);
    }

    if(ty->kind == TY_ENUM && consume_token(TK_SEMICORON)){
        // 列挙型の登録を行う
        return new_node(ND_VOID_STMT, NULL, NULL);
    }

    Ident* ident = declare(qty, sck);
    Node* node = NULL;
    if(sck == SCK_TYPEDEF){
        register_typedef(ident, ident->qtype);
        node = new_node(ND_VOID_STMT, NULL, NULL);
    } else {
        // グローバルスコープならID_GVAR、それ以外はID_LVAR
        bool is_global = get_current_scope() == get_global_scope();
        ident->kind = is_global ? ID_GVAR : ID_LVAR;
        register_ident(ident);

        if(consume_token(TK_ASSIGN)){
            Node* var_node = new_node_var(ident);
            var_node->pos = ident->tok;

            Initializer* init = initialize(ident->qtype, var_node);
            node = init->init_node;

            if(ident->kind == ID_GVAR){
                ident->reloc = make_relocation(init, ident->qtype);
                node = NULL;
            }
        }
    }

    expect_token(TK_SEMICORON);
    return node;
}

static Relocation* make_relocation(Initializer* init, QualType* qty){

    switch(get_qtype_kind(qty)){
        case TY_STRUCT:
        {
            Relocation head = {};
            Relocation* cur = &head;
            Node* block_stmt = init->init_node->body->next;     // blockの1個目はND_MEMZEROなので飛ばす

            int offset = 0;

            for(Member* mem = qty->type->member; mem; mem = mem->next){
                if(offset != mem->ident->offset){
                    // メンバのオフセットがずれているのでパディングを入れないといけない
                    // パディングを入れたことを前提としてメンバのオフセット外れているので、
                    // 単純に差分のサイズの0を入れてあげればいい
                    Relocation* reloc = calloc(1, sizeof(Relocation));
                    reloc->data = 0;
                    reloc->size = abs(offset - mem->ident->offset);
                    reloc->is_padding = true;

                    offset += reloc->size;
                    cur->next = reloc;
                    cur = cur->next;
                }

                if(!block_stmt){
                    if(offset != get_qtype_size(qty)){
                        // 初期化式がもうないが、構造体のサイズに満たない場合は、
                        // 残りのサイズ分のリロケーション情報を追加する
                        Relocation* reloc_remain = calloc(1, sizeof(Relocation));
                        reloc_remain->data = 0;
                        reloc_remain->size = abs(get_qtype_size(qty) - offset);
                        reloc_remain->is_padding = true;   // パディングではないが、.zeroで入れてほしいのでパディング扱いする
                        cur->next = reloc_remain;
                        cur = cur->next;
                        break;
                    }
                }

                Relocation* reloc = calloc(1, sizeof(Relocation));
                if(!is_equal_token(mem->ident->tok, block_stmt->lhs->pos)){
                    // メンバの初期化が飛んでいるので、このメンバはゼロで初期化する
                    reloc->data = 0;
                    reloc->size = get_qtype_size(mem->ident->qtype);
                } else {
                    // メンバの初期化があるので、初期化の値を取得する
                    reloc->data = emit2(block_stmt->rhs, &(reloc->label));
                    reloc->size = get_qtype_size(mem->ident->qtype);

                    // メンバの初期化を消化したので、block_stmtを次に進める
                    if(block_stmt->next){
                        block_stmt = block_stmt->next;
                    }
                }

                // 次の準備
                offset += get_qtype_size(mem->ident->qtype);
                cur->next = reloc;
                cur = cur->next;
            }
            return head.next;
        }
        case TY_ARRAY:
        {
            // Initializerの子のRelocationをすべて取得する
            Relocation head = {};
            Relocation* cur = &head;
            int idx = 0;
            for(Initializer* child = init->child; child; child = child->next){
                cur->next = make_relocation(child, child->qtype);
                cur = cur->next;
                idx++;
            }

            if(idx < qty->type->array_len){
                // 配列の初期化が足りないので、残りのサイズ分のパディングを入れる
                Relocation* reloc_remain = calloc(1, sizeof(Relocation));
                reloc_remain->data = 0;
                reloc_remain->size = get_qtype_size(qty) * qty->type->array_len - idx * get_qtype_size(qty->type->ptr_to);
                reloc_remain->is_padding = true;   // パディングではないが、.zeroで入れてほしいのでパディング扱いする
                cur->next = reloc_remain;
            }
            return head.next;
        }
        default:
        {
            Relocation* reloc = calloc(1, sizeof(Relocation));
            reloc->data = emit2(init->init_node->rhs, &(reloc->label));
            reloc->size = get_qtype_size(init->qtype);
            return reloc;
        }
    }
}

static Initializer* initialize(QualType* ty, Node* var_node){
    Initializer* init = calloc(1, sizeof(Initializer));

    switch(get_qtype_kind(ty)){
        case TY_STRUCT:
        {
            if(consume_token(TK_L_BRACKET)){
                Node* block = new_node(ND_BLOCK, NULL, NULL);

                // 構造体のメンバを初期化する
                Node* head = new_node_memzero(var_node);
                Node* cur = head;

                // 初期化できる対象は構造体のメンバしかないので、
                // 構造体のメンバの数だけループを回せばいい
                for(Member* mem = ty->type->member; mem; mem = mem->next){
                    Token* error_pos = get_token();
                    if(consume_token(TK_R_BRACKET)){
                        // 構造体の初期化が終わった
                        break;
                    }

                    if(consume_token(TK_COMMA)){
                        // 初期化の値を読む前にカンマが来るのはおかしい
                        error_tok(error_pos, "expected expression token before ',' token.\n");
                    }

                    if(consume_token(TK_DOT)){
                        Token* tok = expect_ident();
                        bool is_find = false;
                        for(Member* fm = mem; fm; fm = fm->next){
                            if(is_equal_token(tok, fm->ident->tok)){
                                mem = fm;
                                is_find = true;
                                break;
                            }
                        }

                        if(!is_find){
                            char* struct_name = get_token_string(ty->type->name);
                            char* member_name = get_token_string(tok);
                            error_tok(error_pos, "'%s' has no specific member is '%s'.\n", struct_name, member_name);
                        } else {
                            // メンバの初期化を行うので、イコールが来るはず
                            expect_token(TK_ASSIGN);
                        }
                    }

                    // 今から初期化するメンバの型を取得
                    QualType* mem_qty = mem->ident->qtype;

                    // 代入する値を取得
                    Node* val_node = assign();
                    cur->next = new_node(ND_ASSIGN, new_node_member(var_node, mem->ident), val_node);
                    cur = cur->next;

                    // 次に移行するのでカンマを読む
                    if(!consume_token(TK_COMMA)){
                        // カンマが来ない場合は初期化は終わり
                        // 次は}が来るはず
                        expect_token(TK_R_BRACKET);
                        break;
                    }
                }
                block->body = head;

                init->qtype = ty;
                init->init_node = block;
            }
        }
            break;
        case TY_ARRAY:
        {
            expect_token(TK_L_BRACKET);

            long len = ty->type->array_len;
            bool is_len_known = true;
            if(len < 0){
                len = __INT_MAX__; // 長さが不明な場合は無限大とする
                is_len_known = false;
            }

            if(consume_token(TK_R_BRACKET)){
                // 配列の初期化が空の場合は、全て0で初期化する
                init->qtype = ty;
                init->init_node = new_node(ND_MEMZERO, var_node, NULL);
                return init;
            }

            // --------------------------
            //  配列の初期化子を読み取る
            // --------------------------
            Initializer head = { 0 };
            Initializer* cur = &head;
            int cnt_initialized = 0;
            for(int i = 0; i < len; i++){
                cur->next = initialize(ty->type->ptr_to, var_node);
                cnt_initialized++;
                cur = cur->next;

                if(consume_token(TK_R_BRACKET)){
                    // 配列の初期化が終わった
                    break;
                }

                expect_token(TK_COMMA);
                if(consume_token(TK_R_BRACKET)){
                    // 配列の初期化が終わった
                    // ※最後にカンマが来る場合もあるので、ここでチェックする
                    break;
                }
            }
            init->child = head.next;

            // --------------------------
            //  配列の初期化子から、Relocationを作る
            // --------------------------
            Node* block = new_node(ND_BLOCK, NULL, NULL);

            int idx = 0;
            Node head_node = { 0 };
            Node* cur_node = &head_node;
            cur = head.next;
            for(Initializer* cur_init = init->child; cur_init; cur_init = cur_init->next, idx++){
                // lhs
                Node* arr_node = new_node_add(var_node, new_node_num(idx));
                Node* lhs = new_node(ND_DREF, arr_node, NULL);

                Node* node = new_node(ND_ASSIGN, lhs, cur->init_node->rhs);

                cur_node->next = node;
                cur_node = cur_node->next;

                cur = cur->next;
            }
            if(is_len_known && cnt_initialized < len){
                // 初期化子の数が配列の長さよりも少ない場合は、残りの要素は0で初期化する
                // ※長さが省略されているときは何もしない
                for(int i = cnt_initialized; i < len; i++){
                    Node* arr_node = new_node_add(var_node, new_node_num(i));
                    Node* lhs = new_node(ND_DREF, arr_node, NULL);
                    Node* node = new_node(ND_ASSIGN, lhs, new_node_num(0));

                    cur_node->next = node;
                    cur_node = cur_node->next;
                }
            }

            block->body = head_node.next;

            init->init_node = block;
        }
            break;
        case TY_UNION:
            error("not implemented initializer, struct, union.\n");
            break;
        default:
            init->init_node = new_node(ND_ASSIGN, var_node, assign());
            init->qtype = ty;
            break;
    }
    return init;
}

static QualType* array_type_suffix(QualType* qty){

    // 長さ省略の場合
    if(consume_token(TK_R_SQUARE_BRACKET)){
        qty = type_suffix(qty);
        return array_of(qty, -1);
    }

    // 長さがある場合
    int len = expect_num();
    expect_token(TK_R_SQUARE_BRACKET);
    qty = type_suffix(qty);

    return array_of(qty, len);
}

static QualType* type_suffix(QualType* qty){
    if(consume_token(TK_L_SQUARE_BRACKET)){
        return array_type_suffix(qty);
    }

    return qty;
}

static Ident* declare(QualType* qty, StorageClassKind sck){
    while(consume_token(TK_MUL)){
        qty = pointer_to(qty);
    }

    Token* ident_tok = expect_ident();
    qty = type_suffix(qty);

    Ident* ident = make_ident(ident_tok, ID_LVAR, qty);
    if(sck == SCK_EXTERN){
        ident->is_extern = true;
        ident->kind = ID_GVAR;
    } else if(sck == SCK_STATIC){
        ident->is_static = true;
    }

    return ident;
}


static void count_decl_spec(int* type_flg, int flg, Token* tok){
    // error check
    int target = 0;
    switch(flg){
        case K_LONG:
            // 'long' keyword can use up to 2 times in declaration.
            // ex) long long int
            target = *type_flg & (K_LONG << 1);
            break;
        default:
            target = *type_flg & flg;
            break;
    }

    if(target){
        error_tok(tok, "duplicate type keyword.\n");
    }

    *type_flg += flg;
}

static bool check_storage_class_keyword(StorageClassKind* sck, Token* tok){
    if(consume_token(TK_TYPEDEF)){
        if(*sck != SCK_NONE){
            error_tok(tok, "multiple storage classes in declaration specifies.");
        }
        *sck = SCK_TYPEDEF;
        return true;
    }
    if(consume_token(TK_EXTERN)){
        if(*sck != SCK_NONE){
            error_tok(tok, "multiple storage classes in declaration specifies.");
        }
        *sck = SCK_EXTERN;
        return true;
    }

    if(consume_token(TK_STATIC)){
        if(*sck != SCK_NONE){
            error_tok(tok, "multiple storage classes in declaration specifies.");
        }
        *sck = SCK_STATIC;
        return true;
    }

    // these keyword is recognized but ignored.
    if(consume_token(TK_AUTO)|| consume_token(TK_REGISTER)){
        return true;
    }

    return false;
}

static QualType* declspec(StorageClassKind* sck){

    int type_flg = 0;
    bool is_const = false;
    bool is_restrict = false;
    bool is_volatile = false;
    SimpleType* ty = 0;
    while(is_type()){
        Token* tok = get_token();

        // check storage class keyword.
        if(check_storage_class_keyword(sck, tok)){
            continue;
        }

        if(consume_token(TK_STRUCT) || consume_token(TK_UNION)){
            if(ty || type_flg){
                error_tok(tok, "duplicate type keyword.\n");
            }
            ty = struct_or_union_spec(tok->kind == TK_UNION);
            type_flg += K_USER;
            continue;
        }

        if(consume_token(TK_ENUM)){
            if(ty || type_flg){
                error_tok(tok, "duplicate type keyword.\n");
            }
            ty = enum_spec();
            type_flg += K_USER;
            continue;
        }

        if(consume_token(TK_CONST)){
            if(is_const) error_tok(tok, "duplicate type keyword.\n");
            is_const = true;
            continue;
        }

        if(consume_token(TK_RESTRICT)){
            if(is_restrict) error_tok(tok, "duplicate type keyword.\n");
            is_restrict = true;
            continue;
        }

        if(consume_token(TK_VOLATILE)){
            if(is_volatile) error_tok(tok, "duplicate type keyword.\n");
            is_volatile = true;
            continue;
        }

        // check user type
        Token* typedef_name = consume_typedef_name();
        if(typedef_name){
            if(ty || type_flg){
                error_tok(tok, "duplicate type keyword.\n");
            }
            ty = find_typedef(typedef_name)->qtype->type;
            type_flg += K_USER;
            continue;
        }

        if(consume_token(TK_BOOL))
            count_decl_spec(&type_flg, K_BOOL, tok);
        if(consume_token(TK_VOID))
            count_decl_spec(&type_flg, K_VOID, tok);
        if(consume_token(TK_CHAR))
            count_decl_spec(&type_flg, K_CHAR, tok);
        if(consume_token(TK_SHORT))
            count_decl_spec(&type_flg, K_SHORT, tok);
        if(consume_token(TK_INT))
            count_decl_spec(&type_flg, K_INT, tok);
        if(consume_token(TK_LONG))
            count_decl_spec(&type_flg, K_LONG, tok);
        if(consume_token(TK_SIGNED))
            count_decl_spec(&type_flg, K_SIGNED, tok);
        if(consume_token(TK_UNSIGNED))
            count_decl_spec(&type_flg, K_UNSIGNED, tok);
    }

    if(!ty){
        switch(type_flg){
            case K_VOID:
                ty = ty_void;
                break;
            case K_BOOL:
                ty = ty_bool;
                break;
            case K_CHAR:
            case K_SIGNED + K_CHAR:
                ty = ty_char;
                break;
            case K_UNSIGNED + K_CHAR:
                ty = ty_uchar;
                break;
            case K_SHORT:
            case K_SHORT + K_INT:
            case K_SIGNED + K_SHORT:
            case K_SIGNED + K_SHORT + K_INT:
                ty = ty_short;
                break;
            case K_UNSIGNED + K_SHORT:
            case K_UNSIGNED + K_SHORT + K_INT:
                ty = ty_ushort;
                break;
            case K_INT:
            case K_SIGNED:
            case K_SIGNED + K_INT:
                ty = ty_int;
                break;
            case K_UNSIGNED:
            case K_UNSIGNED + K_INT:
                ty = ty_uint;
                break;
            case K_LONG:
            case K_LONG + K_INT:
            case K_SIGNED + K_LONG:
            case K_SIGNED + K_LONG + K_INT:
                ty = ty_long;
                break;
            case K_UNSIGNED + K_LONG:
            case K_UNSIGNED + K_LONG + K_INT:
                ty = ty_ulong;
                break;
            default:
                error_tok(get_token(), "Invalid type.\n");
                break;
        }
    }

    QualType* qty = make_qual_type(ty);
    if(is_const){
        qty->is_const = true;
    }

    return qty;
}

static Member* struct_or_union_member(){
    Member head;
    Member* cur = &head;
    do {
        cur = cur->next = calloc(1, sizeof(Member));
        StorageClassKind sck = 0;
        QualType* qty = declspec(&sck);
        cur->ident = declare(qty, sck);
        expect_token(TK_SEMICORON);
    } while(!consume_token(TK_R_BRACKET));
    return head.next;
}

static SimpleType* struct_or_union_spec(bool is_union){
    /*
        1. 名前付き構造体
        struct ident {
            declspec ident;
            ...
        }

        2. 不完全な名前付き構造体
        struct ident;

        3. 無名構造体
        struct {
            declspec ident;
            ...
        }
    */

    Token* tok = consume_ident();
    if(tok){
        SimpleType* ty = find_tag(tok);

        if(consume_token(TK_L_BRACKET)){
            // 名前付き構造体
            if(ty && !ty->is_imcomplete){
                error_tok(tok, "redefinition of struct.");
            } else if(ty && ty->is_imcomplete){
                // すでに宣言されているが、不完全な構造体
                ty->member = struct_or_union_member();
                ty->is_imcomplete = false;

                if(is_union){
                    int max_size = 0;
                    for(Member* cur = ty->member; cur; cur = cur->next){
                        cur->ident->offset = 0;
                        if(max_size < get_qtype_size(cur->ident->qtype)){
                            max_size = get_qtype_size(cur->ident->qtype);
                        }
                    }
                    ty->size = max_size;
                } else {
                    int offset = 0;
                    for(Member* cur = ty->member; cur; cur = cur->next){
                        offset += get_qtype_padding(offset, cur->ident->qtype);
                        cur->ident->offset = offset;
                        offset += get_qtype_size(cur->ident->qtype);
                    }
                    ty->size = offset;
                }

            } else {
                // まだ登録されていない構造体
                ty = new_type(is_union ? TY_UNION : TY_STRUCT, 0);
                ty->member = struct_or_union_member();
                ty->name = tok;
                ty->is_imcomplete = false;

                if(is_union){
                    int max_size = 0;
                    for(Member* cur = ty->member; cur; cur = cur->next){
                        cur->ident->offset = 0;
                        if(max_size < get_qtype_size(cur->ident->qtype)){
                            max_size = get_qtype_size(cur->ident->qtype);
                        }
                    }
                    ty->size = max_size;
                } else {
                    int offset = 0;
                    for(Member* cur = ty->member; cur; cur = cur->next){
                        offset += get_qtype_padding(offset, cur->ident->qtype);
                        cur->ident->offset = offset;
                        offset += get_qtype_size(cur->ident->qtype);
                    }
                    ty->size = offset;
                }
                register_tag(ty);
            }
        } else {
            // すでに作っている構造体か、不完全な名前付き構造体
            if(!ty){
                ty = new_type(is_union ? TY_UNION : TY_STRUCT, 0);
                ty->name = tok;
                ty->is_imcomplete = true;
                register_tag(ty);
            }
        }
        return ty;
    } else {
        // 無名構造体
        SimpleType* ty = new_type(is_union ? TY_UNION : TY_STRUCT, 0);
        expect_token(TK_L_BRACKET);
        ty->member = struct_or_union_member();

            if(is_union){
                int max_size = 0;
                for(Member* cur = ty->member; cur; cur = cur->next){
                    cur->ident->offset = 0;
                    if(max_size < get_qtype_size(cur->ident->qtype)){
                        max_size = get_qtype_size(cur->ident->qtype);
                    }
                }
                ty->size = max_size;
            } else {
                int offset = 0;
                for(Member* cur = ty->member; cur; cur = cur->next){
                    offset += get_qtype_padding(offset, cur->ident->qtype);
                    cur->ident->offset = offset;
                    offset += get_qtype_size(cur->ident->qtype);
                }
                ty->size = offset;
            }
        ty->name = &unnamed_struct_token;
        return ty;
    }
}

static Member* enum_member(){
    // enummember = ident ('=' num)? (',' ident ('=' num)?)* '}'
    Member head;
    Member* cur = &head;
    int val = 0;
    QualType* qty = make_qual_type(ty_int);
    qty->is_const = true;

    cur = cur->next = calloc(1, sizeof(Member));
    Token* tok = expect_ident();
    if(consume_token(TK_ASSIGN)){
        Node* node = cond_expr();
        val = emit(node);
    }
    cur->ident = make_ident(tok, ID_ENUM, qty);
    cur->ident->val = val++;
    register_ident(cur->ident);

    if(!consume_token(TK_R_BRACKET)){
        do {
            expect_token(TK_COMMA);
            Token* tok = consume_ident();
            if(!tok) {
                expect_token(TK_R_BRACKET);
                break;
            }
            cur = cur->next = calloc(1, sizeof(Member));
            if(consume_token(TK_ASSIGN)){
                Node* node = cond_expr();
                val = emit(node);
            }
            cur->ident = make_ident(tok, ID_ENUM, qty);
            cur->ident->val = val++;
            register_ident(cur->ident);
        } while(!consume_token(TK_R_BRACKET));
    }
    return head.next;
}

static SimpleType* enum_spec(){
    Token* tok = consume_ident();
    SimpleType* ty = NULL;

    if(tok){
        ty = find_tag(tok);

        if(consume_token(TK_L_BRACKET)){
            if(ty && !ty->is_imcomplete){
                error_tok(tok, "redefinition of enum.");
            } else if(ty && ty->is_imcomplete){
                ty->member = enum_member();
                ty->is_imcomplete = false;
            } else {
                ty = new_type(TY_ENUM, 4);
                ty->name = tok;
                ty->member = enum_member();
                ty->is_imcomplete = false;
                register_tag(ty);
            }
        } else {
            if(!ty){
                ty = new_type(TY_ENUM, 4);
                ty->name = tok;
                ty->is_imcomplete = true;
                register_tag(ty);
            }
        }
    } else {
        expect_token(TK_L_BRACKET);
        ty = new_type(TY_ENUM, 4);
        ty->name = &unnamed_enum_token;
        ty->member = enum_member();
    }
    return ty;
}

static Node* expr(){
    Node* node = assign();
    while(consume_token(TK_COMMA)){
        node = new_node(ND_COMMA, node, assign());
    }
    return node;
}
static int emit(Node* expr){
    char* label = NULL;
    return emit2(expr, &label);
}

static int emit2(Node* expr, char** label){
    int val = 0;

    switch(expr->kind){
        case ND_ADD:
            val = emit(expr->lhs) + emit(expr->rhs);
            break;
        case ND_SUB:
            val = emit(expr->lhs) - emit(expr->rhs);
            break;
        case ND_MUL:
            val = emit(expr->lhs) * emit(expr->rhs);
            break;
        case ND_DIV:
            val = emit(expr->lhs) / emit(expr->rhs);
            break;
        case ND_MOD:
            val = emit(expr->lhs) % emit(expr->rhs);
            break;
        case ND_EQUAL:
            val = emit(expr->lhs) == emit(expr->rhs);
            break;
        case ND_NOT_EQUAL:
            val = emit(expr->lhs) != emit(expr->rhs);
            break;
        case ND_LT:
            val = emit(expr->lhs) < emit(expr->rhs);
            break;
        case ND_LE:
            val = emit(expr->lhs) <= emit(expr->rhs);
            break;
        case ND_BIT_AND:
            val = emit(expr->lhs) & emit(expr->rhs);
            break;
        case ND_BIT_OR:
            val = emit(expr->lhs) | emit(expr->rhs);
            break;
        case ND_BIT_XOR:
            val = emit(expr->lhs) ^ emit(expr->rhs);
            break;
        case ND_LOGIC_AND:
            val = emit(expr->lhs) && emit(expr->rhs);
            break;
        case ND_LOGIC_OR:
            val = emit(expr->lhs) || emit(expr->rhs);
            break;
        case ND_L_BITSHIFT:
            val = emit(expr->lhs) << emit(expr->rhs);
            break;
        case ND_R_BITSHIFT:
            val = emit(expr->lhs) >> emit(expr->rhs);
            break;
        case ND_COND_EXPR:
            val = emit(expr->cond) ? emit(expr->lhs) : emit(expr->rhs);
            break;
        case ND_NUM:
            val = expr->val;
            break;
        case ND_ADDR:
        {
            // アドレス取得先がグローバル変数ならOK
            Node* var = expr->lhs;
            if(var->kind == ND_VAR && var->ident->kind == ID_GVAR){
                if(label){
                    if(var->ident->is_static){
                        *label = format_string(".L%s.%d", var->ident->name, var->ident->static_id);
                    } else {
                        *label = var->ident->name;
                    }
                }
            } else {
                error_tok(expr->pos, "invalid const expression.\n");
            }
            break;
        }
        case ND_VAR:
        {
            if(expr->ident->is_string_literal){
                if(label){
                    *label = expr->ident->name;
                }
            }
            break;
        }
        default:
            error_tok(expr->pos, "invalid const expression.\n");
            break;
    }
    return val;
}

static Node* assign(){
    Node* node = cond_expr();
    add_type(node);

    Token* tok = get_token();

    if(consume_token(TK_ASSIGN)){
        node = new_node(ND_ASSIGN, node, assign());
    } else if(consume_token(TK_PLUS_EQUAL)){
        node = new_node(ND_ASSIGN, node, new_node_add(node, assign()));
    } else if(consume_token(TK_MINUS_EQUAL)){
        node = new_node(ND_ASSIGN, node, new_node_sub(node, assign()));
    } else if(consume_token(TK_MUL_EQUAL)){
        node = new_node(ND_ASSIGN, node, new_node_mul(node, assign()));
    } else if(consume_token(TK_DIV_EQUAL)){
        node = new_node(ND_ASSIGN, node, new_node_div(node, assign()));
    } else if(consume_token(TK_PERCENT_EQUAL)){
        node = new_node(ND_ASSIGN, node, new_node_mod(node, assign()));
    } else if(consume_token(TK_L_BITSHIFT_EQUAL)){
        node = new_node(ND_ASSIGN, node, new_node(ND_L_BITSHIFT, node, assign()));
    } else if(consume_token(TK_R_BITSHIFT_EQUAL)){
        node = new_node(ND_ASSIGN, node, new_node(ND_R_BITSHIFT, node, assign()));
    }

    if(node->kind == ND_ASSIGN){
        if(node->lhs->qtype->is_const){
            error_tok(tok, "cannot assign to const.");
        }
    }

    add_type(node);
    return node;
}

static Node* cond_expr(){
    Node* node = logicOr();
    Token* tok = get_token();
    if(consume_token(TK_QUESTION)){
        Node* cnode = new_node(ND_COND_EXPR, NULL, NULL);
        cnode->cond = node;
        cnode->lhs = expr();
        expect_token(TK_COLON);
        cnode->rhs = cond_expr();
        node = cnode;
        node->pos = tok;
    }
    return node;
}

static Node* logicOr(){
    Node* node = logicAnd();

    while(true){
        Token* tok = get_token();
        if(consume_token(TK_PIPE_PIPE)){
            node = new_node(ND_LOGIC_OR, node, logicAnd());
            node->pos = tok;
        } else {
            return node;
        }
    }
}

static Node* logicAnd(){
    Node* node = bitOr();

    while(true){
        Token* tok = get_token();
        if(consume_token(TK_AND_AND)){
            node = new_node(ND_LOGIC_AND, node, bitOr());
            node->pos = tok;
        } else {
            return node;
        }
    }
}

static Node* bitOr(){
    Node* node = bitXor();

    while(true){
        Token* tok = get_token();
        if(consume_token(TK_PIPE)){
            node = new_node(ND_BIT_OR, node, bitXor());
            node->pos = tok;
        } else {
            return node;
        }
    }
}

static Node* bitXor(){
    Node* node = bitAnd();

    while(true){
        Token* tok = get_token();
        if(consume_token(TK_HAT)){
            node = new_node(ND_BIT_XOR, node, bitAnd());
            node->pos = tok;
        } else {
            return node;
        }
    }
}

static Node* bitAnd(){
    Node* node = equality();

    while(true){
        Token* tok = get_token();
        if(consume_token(TK_AND)){
            node = new_node(ND_BIT_AND, node, equality());
            node->pos = tok;
        } else {
            return node;
        }
    }
}

static Node* equality(){
    Node* node = relational();

    while(true){
        Token* tok = get_token();
        if(consume_token(TK_EQUAL)){
            node = new_node(ND_EQUAL, node, relational());
            node->pos = tok;
        } else if(consume_token(TK_NOT_EQUAL)){
            node = new_node(ND_NOT_EQUAL, node, relational());
            node->pos = tok;
        } else {
            return node;
        }
    }
}

static Node* relational(){
    Node* node = bitShift();
    while(true){
        Token* tok = get_token();
        if(consume_token(TK_L_ANGLE_BRACKET)){
            node = new_node(ND_LT, node, bitShift());
            node->pos = tok;
        } else if(consume_token(TK_L_ANGLE_BRACKET_EQUAL)){
            node = new_node(ND_LE, node, bitShift());
            node->pos = tok;
        } else if(consume_token(TK_R_ANGLE_BRACKET)){
            node = new_node(ND_LT, bitShift(), node);
            node->pos = tok;
        } else if(consume_token(TK_R_ANGLE_BRACKET_EQUAL)){
            node = new_node(ND_LE, bitShift(), node);
            node->pos = tok;
        } else {
            return node;
        }
    }
}

static Node* bitShift(){
    Node* node = add();

    while(true) {
        Token* tok = get_token();
        if(consume_token(TK_L_BITSHIFT)){
            node = new_node(ND_L_BITSHIFT, node, add());
            node->pos = tok;
        } else if(consume_token(TK_R_BITSHIFT)){
            node = new_node(ND_R_BITSHIFT, node, add());
            node->pos = tok;
        } else {
            return node;
        }
    }
}

static Node* add(){
    Node* node = mul();

    while(true){
        Token* tok = get_token();
        if(consume_token(TK_PLUS)){
            node = new_node(ND_ADD, node, mul());
            node->pos = tok;
        } else if(consume_token(TK_MINUS)){
            node = new_node(ND_SUB, node, mul());
            node->pos = tok;
        } else {
            return node;
        }
    }
}

static Node* mul(){
    Node* node = cast();

    while(true){
        Token* tok = get_token();
        if(consume_token(TK_MUL)){
            node = new_node(ND_MUL, node, cast());
            node->pos = tok;
        } else if(consume_token(TK_DIV)){
            node = new_node(ND_DIV, node, cast());
            node->pos = tok;
        } else if(consume_token(TK_PERCENT)){
            node = new_node(ND_MOD, node, cast());
            node->pos = tok;
        } else {
            break;
        }
    }

    return node;
}

static bool is_cast(){
    bool result = false;
    Token* tok = get_token();

    if(consume_token(TK_L_PAREN)){
        if(is_type()){
            result = true;
        }
    }
    set_token(tok);
    return result;
}

static Node* cast(){
    if(is_cast()){
        Token* tok = get_token();
        expect_token(TK_L_PAREN);
        QualType* qty = declspec(NULL);
        while(consume_token(TK_MUL)){
            qty = pointer_to(qty);
        }
        expect_token(TK_R_PAREN);

        Node* node = cast();
        add_type(node);
        node = new_node(ND_CAST, node, NULL);
        node->qtype = qty;
        node->pos = tok;

        return node;
    }
    return unary();
}

static Node* unary(){
    Token* tok = get_token();
    if(consume_token(TK_PLUS)){
        Node* node = cast();
        node->pos = tok;
        return node;
    } else if(consume_token(TK_MINUS)){
        Node* node = new_node_sub(new_node_num(0), cast());
        node->pos = tok;
        return node;
    } else if(consume_token(TK_AND)){
        Node* node = new_node(ND_ADDR, cast(), NULL);
        node->pos = tok;
        return node;
    } else if(consume_token(TK_MUL)){
        Node* node =  new_node(ND_DREF, cast(), NULL);
        node->pos = tok;
        return node;
    } else if(consume_token(TK_NOT)){
        Node* node =  new_node(ND_NOT, cast(), NULL);
        node->pos = tok;
        return node;
    } else if(consume_token(TK_PLUS_PLUS)){
        Node* node = unary();
        node = new_node(ND_ASSIGN, node, new_node_add(node, new_node_num(1)));
        node->pos = tok;
        return node;
    } else if(consume_token(TK_MINUS_MINUS)){
        Node* node = unary();
        node = new_node(ND_ASSIGN, node, new_node_sub(node, new_node_num(1)));
        node->pos = tok;
        return node;
    } else if(consume_token(TK_SIZEOF)){
        bool is_l_paren = consume_token(TK_L_PAREN);
        Node* node = NULL;
        if(is_type()){
            StorageClassKind sck = 0;
            QualType* qty = declspec(&sck);
            while(consume_token(TK_MUL)){
                qty = pointer_to(qty);
            }
            node = new_node_num(get_qtype_size(qty));
        } else {
            node = unary();
            add_type(node);
            if(get_qtype_kind(node->qtype) == TY_ARRAY){
                node = new_node_num(get_qtype_size(node->qtype));
            } else if(get_qtype_kind(node->qtype) == TY_STRUCT || get_qtype_kind(node->qtype) == TY_UNION){
                node = new_node_num(get_qtype_size(node->qtype));
            } else {
                node = new_node_num(get_qtype_size(node->qtype));
            }
        }
        if(is_l_paren){
            expect_token(TK_R_PAREN);
        }
        return node;
    }
    return postfix();
}

static Node* postfix(){
    Node* node = primary();

    while(true){
        if(consume_token(TK_L_SQUARE_BRACKET)){
            node = new_node(ND_DREF, new_node_add(node, expr()), NULL);
            expect_token(TK_R_SQUARE_BRACKET);
            continue;
        }

        if(consume_token(TK_PLUS_PLUS)){
            node = new_inc(node);
            continue;
        }

        if(consume_token(TK_MINUS_MINUS)){
            node = new_dec(node);
            continue;
        }

        if(consume_token(TK_DOT)){
            add_type(node);

            // find a member
            Token* tok = expect_ident();
            Ident* ident = get_member(node->qtype->type, tok);
            if(!ident){
                error_tok(tok, "Not a member.\n");
            }

            node = new_node_member(node, ident);
            continue;
        }

        if(consume_token(TK_ARROW)){
            add_type(node);
            node = new_node(ND_DREF, node, NULL);
            add_type(node);
            node = new_node(ND_MEMBER, node, NULL);

            // find a member
            Token* tok = expect_ident();
            Ident* ident = get_member(node->lhs->qtype->type, tok);
            if(!ident){
                error_tok(tok, "Not a member.\n");
            }

            node->qtype = ident->qtype;
            node->val = ident->offset;
            continue;
        }

        return node;
    }
}

static Node* primary(){
    Token* pos_tok = get_token();

    if(consume_token(TK_L_PAREN)){
        Node* node = expr();
        expect_token(TK_R_PAREN);
        return node;
    }

    // 文字列リテラル
    Token* tok_str = consume_string_literal();
    if(tok_str){
        Ident* str_ident = register_string_literal(tok_str);
        Node* node = new_node(ND_VAR, 0, 0);
        node->ident = str_ident;
        node->qtype = str_ident->qtype;
        return node;
    }

    // ビルトイン関数
    if(consume_token(TK_VA_START)){
        // 第１第２引数を取得
        expect_token(TK_L_PAREN);
        Node* lhs_node = assign();
        expect_token(TK_COMMA);
        Node* rhs_node = assign();
        expect_token(TK_R_PAREN);

        Node* node = new_node(ND_VA_START, lhs_node, NULL);
        node->pos = pos_tok;
        return node;
    }

    if(consume_token(TK_VA_ARG)){
        expect_token(TK_L_PAREN);
        Node* arg1_node = assign();
        expect_token(TK_COMMA);
        if(!is_type()){
            // ここは型を受け取るべきところ
            error_tok(pos_tok, "expected type keyword or specific type.");
        }
        StorageClassKind sck = 0;
        QualType* qtype = declspec(NULL);
        expect_token(TK_R_PAREN);

        if(is_integer_type(qtype)){
            // TODO : 引数渡ししか対応していないので、そのようにする

            // gp_offsetを見つける
            arg1_node = new_node(ND_DREF, arg1_node, NULL);
            add_type(arg1_node);
            Node* gp_offset_node = new_node(ND_MEMBER, arg1_node, NULL);
            Ident* gp_offset_ident = get_member(arg1_node->qtype->type, &va_elem_gp_offset_token);
            if(!gp_offset_ident){
                error_tok(pos_tok, "[Internal Error] Not found gp_offset.\n");
            }
            gp_offset_node->qtype = gp_offset_ident->qtype;
            gp_offset_node->val = gp_offset_ident->offset;

            // reg_save_areaを見つける
            Node* reg_save_area_node = new_node(ND_MEMBER, arg1_node, NULL);
            Ident* reg_save_area_ident = get_member(arg1_node->qtype->type, &va_elem_reg_save_area_token);
            if(!reg_save_area_ident){
                error_tok(pos_tok, "[Internal Error] Not found reg_save_area.\n");
            }
            reg_save_area_node->qtype = reg_save_area_ident->qtype;
            reg_save_area_node->val = reg_save_area_ident->offset;

            // gp_offset+=8, *(long*)(reg_save_area + (gp_offset - 8))
            Node* gp_offset_inc = new_node(ND_ASSIGN, gp_offset_node, new_node_add(gp_offset_node, new_node_num(8)));

            Node* calc_add = new_node_add(reg_save_area_node, new_node_sub(gp_offset_node, new_node_num(8)));
            Node* cast_adr = new_node(ND_CAST, calc_add, NULL);
            cast_adr->qtype = pointer_to(make_qual_type(ty_long));
            Node* va_arg_node = new_node(ND_DREF, cast_adr, NULL);

            Node* canma_node = new_node(ND_COMMA, gp_offset_inc, va_arg_node);
            return canma_node;
        } else {
            error_tok(pos_tok, "Not implemented va_arg to float type.");
        }
    }

    if(consume_token(TK_VA_END)){
        Node* node = new_node(ND_NOP, 0, 0);
        node->qtype = make_qual_type(ty_void);
        node->pos = pos_tok;
        expect_token(TK_L_PAREN);
        Node* arg1_node = assign();
        expect_token(TK_R_PAREN);
        return node;
    }

    // 変数
    Token* ident_token = consume_ident();
    if(ident_token){
        Ident* ident = find_ident(ident_token);
        if(!ident){
            error_tok(ident_token, "Undefined Variable.");
        } else {
            if((ident->kind == ID_LVAR) || (ident->kind == ID_GVAR)){
                Node* node = new_node(ND_VAR, 0, 0);
                node->ident = ident;
                node->qtype = ident->qtype;
                node->pos = ident_token;
                return node;
            } else if(ident->kind == ID_ENUM){
                Node* node = new_node_num(ident->val);
                return node;
            } else if(ident->kind == ID_FUNC){
                if(consume_token(TK_L_PAREN)){
                    Node* node = new_node(ND_FUNCCALL, 0, 0);
                    node->ident = ident;
                    node->qtype = ident->qtype;
                    node->pos = ident_token;
                    if(!consume_token(TK_R_PAREN)){
                        Node head = {};
                        Node* nd_param = &head;
                        do{
                            nd_param = nd_param->next = assign();
                        } while(consume_token(TK_COMMA));
                        expect_token(TK_R_PAREN);
                        node->params = head.next;
                    }
                    return node;
                }
                unreachable();
            }
        }
    }

    // 数値リテラル
    unsigned long val = expect_num();
    Node* node_num = new_node_num(val);
    node_num->pos = pos_tok;
    return node_num;
}

static Node* const_expr(){
    Token* tok = get_token();
    Node* node = expr();
    if(node->kind != ND_NUM){
        error_tok(tok, "expected constant expression.");
    }
    return node;
}

static Node* new_node(NodeKind kind, Node* lhs, Node* rhs){
    Node* result = calloc(1, sizeof(Node));
    result->kind = kind;
    result->lhs = lhs;
    result->rhs = rhs;
    return result;
}

static Node* new_node_num(unsigned long num){
    Node* result = calloc(1, sizeof(Node));
    result->kind = ND_NUM;
    result->val = num;
    return result;
}

static Node* new_node_var(Ident* ident){
    Node* result = calloc(1, sizeof(Node));
    result->kind = ND_VAR;
    result->ident = ident;
    result->qtype = ident->qtype;

    return result;
}

static Node* new_node_memzero(Node* var_node){
    Node* result = new_node(ND_MEMZERO, var_node, NULL);
    result->qtype = var_node->qtype;
    return result;
}

static Node* new_node_member(Node* var_node, Ident* member){
    Node* result = new_node(ND_MEMBER, var_node, NULL);
    result->pos = member->tok;
    result->qtype = member->qtype;
    result->val = member->offset;
    return result;
}

static Node* new_node_add(Node* lhs, Node* rhs){

    add_type(lhs);
    add_type(rhs);

    Node* result = new_node(ND_ADD, lhs, rhs);

    // num + num
    if(!get_qtype_ptr_to(lhs->qtype) && !get_qtype_ptr_to(rhs->qtype))
    {
        return result;
    }
    
    // pointer + pointer
    if(get_qtype_ptr_to(lhs->qtype) && get_qtype_ptr_to(rhs->qtype)){
        error("Try add pointer and pointer.");
    }
    
    // num + pointer
    if(!get_qtype_ptr_to(lhs->qtype) && get_qtype_ptr_to(rhs->qtype)){
        // ポインタ + 数値、になるように入れ替える
        Node* buf = lhs;
        rhs = lhs;
        lhs = buf;
    }

    QualType* ptr_to = get_qtype_ptr_to(lhs->qtype);
    Node* node_mul = new_node_mul(rhs, new_node_num( get_qtype_size(ptr_to)));
    result->lhs = lhs;
    result->rhs = node_mul;

    return result;
}
static Node* new_node_sub(Node* lhs, Node* rhs){

    add_type(lhs);
    add_type(rhs);

    Node* result = new_node(ND_SUB, lhs, rhs);

    // num + num
    if(!get_qtype_ptr_to(lhs->qtype) && !get_qtype_ptr_to(rhs->qtype))
    {
        return result;
    }

    // - pointer
    if(get_qtype_ptr_to(rhs->qtype)){
        error("invalid operand.");
    }

    // pointer - num
    if(get_qtype_ptr_to(lhs->qtype) && !get_qtype_ptr_to(rhs->qtype)){
        QualType* ptr_to = get_qtype_ptr_to(lhs->qtype);
        Node* node_mul = new_node_mul(rhs, new_node_num(get_qtype_size(ptr_to)));
        result->lhs = lhs;
        result->rhs = node_mul;
        return result;
    }
}
static Node* new_node_div(Node* lhs, Node* rhs){
    Node* result = new_node(ND_DIV, lhs, rhs);
    return result;
}
static Node* new_node_mul(Node* lhs, Node* rhs){
    Node* result = new_node(ND_MUL, lhs, rhs);
    return result;
}
static Node* new_node_mod(Node* lhs, Node* rhs){
    Node* result = new_node(ND_MOD, lhs, rhs);
    return result;
}

static Node* new_inc(Node* var){
    Token tok;
    tok.pos = "tmp";
    tok.len = 3;
    tok.kind = TK_IDENT;
    scope_in();
    QualType* qty = calloc(1, sizeof(QualType));
    memcpy(qty, var->qtype, sizeof(QualType));

    Ident* tmp = declare_ident(&tok, ID_LVAR, qty);

    Node* node_tmp = new_node_var(tmp);
    Node* node_assign = new_node(ND_ASSIGN, node_tmp, var);
    Node* node_inc = new_node(ND_ASSIGN, var, new_node_add(var, new_node_num(1)));
    Node* node = new_node(ND_COMMA, node_assign, new_node(ND_COMMA, node_inc, node_tmp));

    scope_out();
    return node;
}

static Node* new_dec(Node* var){
    Token tok;
    tok.pos = "tmp";
    tok.len = 3;
    tok.kind = TK_IDENT;
    scope_in();
    QualType* qty = calloc(1, sizeof(QualType));
    memcpy(qty, var->qtype, sizeof(QualType));

    Ident* tmp = declare_ident(&tok, ID_LVAR, qty);

    Node* node_tmp = new_node_var(tmp);
    Node* node_assign = new_node(ND_ASSIGN, node_tmp, var);
    Node* node_inc = new_node(ND_ASSIGN, var, new_node_sub(var, new_node_num(1)));
    Node* node = new_node(ND_COMMA, node_assign, new_node(ND_COMMA, node_inc, node_tmp));

    scope_out();
    return node;
}

static bool is_function(){
    Token* bkup = get_token();
    bool retval = false;

    // ポインタの読み取り
    while(consume_token(TK_MUL)){
        ;
    }

    expect_token(TK_IDENT);
    if(consume_token(TK_L_PAREN)){
        retval = true;
    }

    set_token(bkup);
    return retval;
}

// 可変長引数関数か探る
//  fund(int a, int b, ...)
//      ^ ここで呼ばれる前提
static bool is_var_func(){
    Token* bkup = get_token();
    bool retval = false;

    while(!consume_token(TK_R_PAREN)){
        if(consume_token(TK_DOT_DOT_DOT)){
            retval = true;
            break;
        }
        token = token->next;
    }
    set_token(bkup);
    return retval;
}

// ----------------------------
// トークン操作
void expect_token(TokenKind kind){
    if(token->kind != kind){
        error_tok(token, "error: unexpected token.\n");
    }

    token = token->next;
}

unsigned long expect_num(){
    if(token->kind != TK_NUM){
        error_tok(token, "error: not a number.\n", token->pos);
    }

    unsigned long result = token->val;
    token = token->next;
    return result;
}

bool consume_token(TokenKind kind){
    if(token->kind != kind){
        return false;
    }
    token = token->next;
    return true;
}

Token* consume_ident(){
    if(token->kind != TK_IDENT) return NULL;

    Token* tok = token;
    token = token->next;
    return tok;
}

Token* consume_string_literal(){
    if(token->kind != TK_STRING_LITERAL) return NULL;
    Token* tok = token;
    token = token->next;
    return tok;
}

Token* consume_typedef_name(){
    if(token->kind != TK_IDENT) return NULL;
    Ident* ident = find_typedef(token);
    if(ident == NULL) return NULL;
    Token* typedef_token = token;
    token = token->next;
    return typedef_token;
}

Token* expect_ident(){
    Token* tok = consume_ident();
    if(tok == NULL){
        error_tok(token, "error: not a ident.\n", token->pos);
    }
    return tok;
}

bool is_eof(){
    return token->kind == TK_EOF;
}


bool is_type(){

    // is token registerd as typedef name?
    Ident* ident = find_typedef(token);
    if(ident){
        return true;
    }

    return token->kind == TK_STRUCT
        || token->kind == TK_UNION
        || token->kind == TK_ENUM
        || token->kind == TK_CONST
        || token->kind == TK_VOLATILE
        || token->kind == TK_RESTRICT
        || token->kind == TK_SIGNED
        || token->kind == TK_UNSIGNED
        || token->kind == TK_TYPEDEF
        || token->kind == TK_AUTO
        || token->kind == TK_REGISTER
        || token->kind == TK_RESTRICT
        || token->kind == TK_EXTERN
        || token->kind == TK_STATIC
        || token->kind == TK_BOOL
        || token->kind == TK_VOID
        || token->kind == TK_INT
        || token->kind == TK_LONG
        || token->kind == TK_SHORT
        || token->kind == TK_CHAR;
}

bool is_label(){
    if(token->kind == TK_IDENT && token->next->kind == TK_COLON){
        return true;
    }
    return false;
}

Token* get_token(){
    return token;
}

void set_token(Token* tok){
    token = tok;
}
