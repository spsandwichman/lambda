#include "lambda.h"

static u32 free_vars = 0;

static bool tokens_eq(Token* a, Token* b) {
    return string_eq(
        (string){a->raw, a->len},
        (string){b->raw, b->len}
    );
}

static u32 get_dbi(AstVar* var) {
    
    u32 index = 1;

    Ast* expr = (Ast*)var;

    while (expr->parent) {
        switch (expr->kind) {
        case AST_LAMBDA:
            if (tokens_eq(((AstLambda*)expr)->arg, var->ident)) {
                var->bind = (AstLambda*)expr;
                return index;
            }
            index++;
            break;
        }
        expr = expr->parent;
    }

    return index + free_vars++;
}

static void i_find_de_bruijn(Ast* expr) {

    switch (expr->kind) {
    case AST_APPLY:
        i_find_de_bruijn(((AstApply*)expr)->func);
        i_find_de_bruijn(((AstApply*)expr)->input);
        break;
    case AST_LAMBDA:
        i_find_de_bruijn(((AstLambda*)expr)->body);
        break;
    case AST_VAR:
        ((AstVar*)expr)->dbi = get_dbi((AstVar*)expr);
    }
}

void de_bruijn(Ast* expr) {
    free_vars = 0;
    i_find_de_bruijn(expr);
}

Ast* eval(Ast* expr) {
    while (expr->kind == AST_APPLY) {
        AstApply* apply = (AstApply*) expr;
        
        
    }
    return expr;
}