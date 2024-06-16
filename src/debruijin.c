#include "lambda.h"

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