#include "lambda.h"

void print_debruijn(Expr* expr) {
    switch(expr->kind) {
    case EXPR_LAM:
        printf("λ ");
        print_debruijn(expr->lam);
        break;
    case EXPR_APP:
        if (expr->app.func->kind == EXPR_LAM) {
            printf("(");
            print_debruijn(expr->app.func);
            printf(")");
        } else {
            print_debruijn(expr->app.func);
        }
        printf(" ");
        if (expr->app.input->kind == EXPR_APP) {
            printf("(");
            print_debruijn(expr->app.input);
            printf(")");
        } else {
            print_debruijn(expr->app.input);
        }
        break;
    case EXPR_VAR:
        printf("%llu", expr->var.index);
        break;
    }
}

static const char* varnames[] = {
    "x", "y", "z", "w",
    "m", "n",
    "u", "v",
    "i", "j", "k",

    "a", "b", "c", "d", "e", 
    "f", "g", "h", "l", "o", 
    "p", "q", "r", "s", "t",
};

void i_print_standard(Expr* expr, u64 lam_depth) {
    switch(expr->kind) {
    case EXPR_LAM:
        printf("λ%s.", varnames[lam_depth % 26]);
        i_print_standard(expr->lam, lam_depth + 1);
        break;
    case EXPR_APP:
        if (expr->app.func->kind == EXPR_LAM) {
            printf("(");
            i_print_standard(expr->app.func, lam_depth);
            printf(")");
        } else {
            i_print_standard(expr->app.func, lam_depth);
        }
        printf(" ");
        if (expr->app.input->kind == EXPR_APP) {
            printf("(");
            i_print_standard(expr->app.input, lam_depth);
            printf(")");
        } else {
            i_print_standard(expr->app.input, lam_depth);
        }
        break;
    case EXPR_VAR:
        if (lam_depth == 0 || (i64)(lam_depth - expr->var.index) < 0) {
            printf("%s", varnames[(expr->var.index - 1) % 26]);
        } else {
            printf("%s", varnames[(lam_depth - expr->var.index) % 26]);
        }
        break;
    }
}

// SUPER buggy
void print_standard(Expr* expr) {
    i_print_standard(expr, 0);
}