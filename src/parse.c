#include "lambda.h"

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
    lam->body->parent = (Ast*)lam;

    if (lam->body == NULL) return NULL;

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
        right->parent = old_left->parent = left;
    }

    return left;
}

Ast* parse(TokenStream* ts) {
    return parse_expr(ts);
}

#define indent(n) printf("%*s", (n)*2, "")

static void i_print_tree(Ast* expr, u32 i) {
    indent(i);

    switch (expr->kind) {
    case AST_LAMBDA:
        printf("λ."str_fmt" =\n", str_arg(*((AstLambda*)expr)->arg));
        i_print_tree(((AstLambda*)expr)->body, i + 1);
        break;
    case AST_APPLY:
        printf("apply\n");
        i_print_tree(((AstApply*)expr)->func, i + 1);
        indent(i);
        printf("to\n");
        i_print_tree(((AstApply*)expr)->input, i + 1);
        break;
    case AST_VAR:
        printf("'"str_fmt"', %u\n", str_arg(*((AstVar*)expr)->ident), ((AstVar*)expr)->dbi);
        break;
    default:
        break;
    }
}

void print_tree(Ast* expr) {
    i_print_tree(expr, 0);
}