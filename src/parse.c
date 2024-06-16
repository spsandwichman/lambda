#include "parse.h"

static forceinline Token* current(TokenStream* ts) {
    return &ts->at[ts->cursor];
}

static forceinline void advance(TokenStream* ts) {
    ts->cursor++;
}

Ast* new_ast(u8 kind) {
    size_t size;

    switch (kind) {
    case AST_LAMBDA: size = sizeof(AstLambda); break;
    case AST_APPLY:  size = sizeof(AstApply);  break;
    case AST_VAR:    size = sizeof(AstVar);    break;
    default: return NULL;
    }

    Ast* n = calloc(1, size);
    n->kind = kind;
    return n;
}

Ast* parse_lambda(TokenStream* ts) {
    AstLambda* lam = (AstLambda*)new_ast(AST_LAMBDA);

    if (current(ts)->kind != TOK_LAMBDA) return NULL;
    advance(ts);

    if (current(ts)->kind != TOK_IDENT) return NULL;
    lam->arg = current(ts);
    advance(ts);

    if (current(ts)->kind != TOK_PERIOD) return NULL;
    advance(ts);

    lam->body = parse_expr(ts);
    if (lam->body == NULL) return NULL;
    lam->body->parent = (Ast*)lam;

    return (Ast*)lam;
}

Ast* parse_term(TokenStream* ts) {
    switch(current(ts)->kind) {
    case TOK_OPEN_PAREN:
        advance(ts);
        Ast* inside = parse_expr(ts);
        if (current(ts)->kind != TOK_CLOSE_PAREN) return NULL;
        advance(ts);
        return inside;
    case TOK_LAMBDA:
        return parse_lambda(ts);
    case TOK_IDENT:
        Ast* var = new_ast(AST_VAR);
        ((AstVar*)var)->ident = current(ts);
        advance(ts);
        return var;
    }

    return NULL;
}

Ast* parse_expr(TokenStream* ts) {
    if (current(ts)->kind == TOK_EOF) return NULL;
    
    Ast* left = parse_term(ts);
    if (left == NULL) return NULL;

    while (current(ts)->kind != TOK_EOF) {
        Ast* right = parse_term(ts);
        if (right == NULL) break;

        Ast* old_left = left;
        left = new_ast(AST_APPLY);
        ((AstApply*)left)->func = old_left;
        ((AstApply*)left)->input = right;
        ((AstApply*)left)->func->parent = left;
        ((AstApply*)left)->input->parent = left;
    }

    return left;
}

void destroy_ast(Ast* expr) {
    switch(expr->kind) {
    case AST_LAMBDA:
        AstLambda* l = (AstLambda*)expr;
        destroy_ast(l->body);
        break;
    case AST_APPLY:
        AstApply* a = (AstApply*)expr;
        destroy_ast(a->func);
        destroy_ast(a->input);
        break;
    case AST_VAR:
        break;
    }
    free(expr);
}

da_typedef(string);

static Expr* new_expr() {
    return calloc(1, sizeof(Expr));
}

#define tokenptr2str(t) ((string){.len = (t)->len, .raw = (t)->raw})

static da(string) free_vars = {0};

static Expr* ast_to_db(Ast* expr, da(string)* func_param_stack) {
    Expr* db = new_expr();
    db->kind = expr->kind;

    switch(expr->kind) {
    case AST_LAMBDA:
        AstLambda* l = (AstLambda*)expr;
        da_append(func_param_stack, tokenptr2str(l->arg)); // push arg onto stack
        db->lam = ast_to_db(l->body, func_param_stack);
        da_pop(func_param_stack); // pop arg off of stack
        break;
    case AST_APPLY:
        AstApply* a = (AstApply*)expr;
        db->app.func  = ast_to_db(a->func,  func_param_stack);
        db->app.input = ast_to_db(a->input, func_param_stack);
        break;
    case AST_VAR:
        AstVar* v = (AstVar*)expr;
        string match = tokenptr2str(v->ident);
        for (db->var.index = 1; db->var.index <= func_param_stack->len; db->var.index++) {
            string arg = func_param_stack->at[func_param_stack->len - db->var.index];
            if (string_eq(arg, match)) {
                return db;
            }
        }
        for (u64 i = 0; i < free_vars.len; i++) {
            string free = free_vars.at[i];
            if (string_eq(free, match)) {
                db->var.index += i;
                return db;
            }
        }
        da_append(&free_vars, match);
        db->var.index += free_vars.len - 1;
        break;
    }
    return db;
}

Expr* to_expr(Ast* expr) {
    da(string) func_param_stack = {0};
    free_vars = (da(string)){0};

    da_init(&func_param_stack, 16);
    da_init(&free_vars, 16);

    Expr* db = ast_to_db(expr, &func_param_stack);
    assert(func_param_stack.len == 0);

    return db;
}

Expr* parse(TokenStream* ts) {
    Ast* ast = parse_expr(ts);
    Expr* expr = to_expr(ast);
    destroy_ast(ast);
    return expr;
}