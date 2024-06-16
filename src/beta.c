#include "lambda.h"

void destroy_expr(Expr* expr) {
    switch(expr->kind) {
    case EXPR_LAM:
        destroy_expr(expr->lam);
        free_expr(expr);
        break;
    case EXPR_APP:
        destroy_expr(expr->app.func);
        destroy_expr(expr->app.input);
        free_expr(expr);
        break;
    case EXPR_VAR:
        free_expr(expr);
        break;
    default:
        UNREACHABLE;
    }
}

bool is_beta_reducible(Expr* expr) {
    switch(expr->kind) {
    case EXPR_LAM:
        return is_beta_reducible(expr->lam);
        break;
    case EXPR_APP:
        if (expr->app.func->kind == EXPR_LAM) {
            return true;
        }
        return is_beta_reducible(expr->app.func) || is_beta_reducible(expr->app.input);
        break;
    case EXPR_VAR:
        return false;
        break;
    }
    UNREACHABLE;
}

int mark_bound(Expr* expr, u64 depth) {
    switch(expr->kind) {
    case EXPR_LAM: return mark_bound(expr->lam, depth+1);
    case EXPR_APP: return mark_bound(expr->app.func, depth) + mark_bound(expr->app.input, depth);
    case EXPR_VAR: return (int)(expr->var.bound = expr->var.index == depth);
    }
    UNREACHABLE;
}

void decrement_free_vars(Expr* expr, u64 depth) {
    switch(expr->kind) {
    case EXPR_LAM:
        decrement_free_vars(expr->lam, depth + 1);
        return;
    case EXPR_APP:
        decrement_free_vars(expr->app.func,  depth);
        decrement_free_vars(expr->app.input, depth);
        return;
    case EXPR_VAR:
        if (expr->var.index > depth) {
            expr->var.index--;
        }
        return;
    }
    UNREACHABLE;
}

Expr* clone_input_and_inc_free_vars(Expr* input, u64 depth, u64 inc) {

    Expr* e = new_expr();
    e->kind = input->kind;

    switch(e->kind) {
    case EXPR_LAM:
        e->lam = clone_input_and_inc_free_vars(input->lam, depth+1, inc);
        break;
    case EXPR_APP:
        e->app.func  = clone_input_and_inc_free_vars(input->app.func,  depth, inc);
        e->app.input = clone_input_and_inc_free_vars(input->app.input, depth, inc);
        break;
    case EXPR_VAR:
        e->var.index = input->var.index;
        if (input->var.index >= depth) {
            e->var.index += inc;
        }
        break;
    default:
        UNREACHABLE;
    }
    return e;
};

Expr* clone_input_to_bound_vars(Expr* body, Expr* input, u64 depth) {
    switch(body->kind) {
    case EXPR_LAM:
        body->lam = clone_input_to_bound_vars(body->lam, input, depth+1);
        return body;
    case EXPR_APP:
        body->app.func  = clone_input_to_bound_vars(body->app.func,  input, depth);
        body->app.input = clone_input_to_bound_vars(body->app.input, input, depth);
        return body;
    case EXPR_VAR:
        if (body->var.bound) {
            free_expr(body);
            return clone_input_and_inc_free_vars(input, 1, depth);
        }
        return body;
        break;
    default:
        UNREACHABLE;
    }
}

Expr* beta_reduce(Expr* expr) {
    // only beta reduce if its an application and our function is a lambda
    // e.g. (λx.x) y
    // not (x y)
    // not (λx.x)
    if ((expr)->kind != EXPR_APP || (expr)->app.func->kind != EXPR_LAM) return expr;

    // (λ M) N
    Expr* M = expr->app.func->lam;
    Expr* N = expr->app.input;

    int bound = mark_bound(M, 1);

    decrement_free_vars(M, 1);

    if (bound != 0) M = clone_input_to_bound_vars(M, N, 0);

    destroy_expr(N);
    free_expr(expr->app.func);
    free_expr(expr);

    return M;
}

// find an opportunity to beta-reduce and take it
bool beta(Expr** expr) {

    Expr* e = *expr;

    switch(e->kind) {
    case EXPR_LAM:
        return beta(&e->lam);
    case EXPR_APP:
        if (e->app.func->kind == EXPR_LAM) {
            *expr = beta_reduce(*expr);
            return true;
        }
        return beta(&e->app.func) || beta(&e->app.input);
        
    case EXPR_VAR:
        return false;
    }
    UNREACHABLE;
}