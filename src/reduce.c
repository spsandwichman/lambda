#include "lambda.h"

void destroy_expr(Expr* expr) {
    switch(expr->kind) {
    case EXPR_LAM:
        destroy_expr(expr->lam);
        alloca_delete(expr);
        break;
    case EXPR_APP:
        destroy_expr(expr->app.func);
        destroy_expr(expr->app.input);
        alloca_delete(expr);
        break;
    case EXPR_VAR:
        alloca_delete(expr);
        break;
    default:
        UNREACHABLE;
    }
}


Expr* clone_input_and_inc_free_vars(Expr* input, u64 depth, u64 inc) {

    Expr* e = alloca_new();
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

// attempt to do everything
Expr* do_all_traversals_lmao(Expr* body, Expr* input, u64 depth) {
    switch(body->kind) {
    case EXPR_LAM:
        body->lam = do_all_traversals_lmao(body->lam, input, depth + 1);
        return body;
    case EXPR_APP:
        body->app.func  = do_all_traversals_lmao(body->app.func,  input, depth);
        body->app.input = do_all_traversals_lmao(body->app.input, input, depth);
        return body;
    case EXPR_VAR:
        bool bound = (body->var.index == depth);
        // decrement free variables
        if (bound) {
            alloca_delete(body);
            return clone_input_and_inc_free_vars(input, 1, depth - 1);
        }
        if (body->var.index > depth) {
            body->var.index--;
        }
        return body;
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

    M = do_all_traversals_lmao(M, N, 1);

    destroy_expr(N);
    alloca_delete(expr->app.func);
    alloca_delete(expr);

    return M;
}

// find an opportunity to beta-reduce and take it
u64 beta(Expr** expr, bool recurse) {

    Expr* e = *expr;

    switch(e->kind) {
    case EXPR_LAM:
        return beta(&e->lam, recurse);
    case EXPR_APP:
        if (!recurse) {

            if (e->app.func->kind == EXPR_LAM) {
                *expr = beta_reduce(*expr);
                return 1;
            }
            u64 fun = beta(&e->app.func, recurse);
            if (fun) return fun;
            return beta(&e->app.input, recurse);
        } else {
            u64 fun = beta(&e->app.func, recurse);
            u64 inp = beta(&e->app.input, recurse);
            if (e->app.func->kind == EXPR_LAM) {
                *expr = beta_reduce(*expr);
                return 1 + fun + inp;
            }
            return fun + inp;
        }       
    case EXPR_VAR:
        return false;
    }
    UNREACHABLE;
}