#pragma once

#include "lambda.h"

enum {
    AST_LAMBDA,
    AST_APPLY,
    AST_VAR,
};

#define _AST_BASE_ \
    struct Ast* parent; \
    u8 kind;

typedef struct Ast {
    _AST_BASE_

} Ast;

// lambda definition
typedef struct AstLambda {
    _AST_BASE_

    Token* arg;
    Ast* body;
} AstLambda;

// function application
typedef struct AstApply {
    _AST_BASE_

    Ast* func;
    Ast* input;
} AstApply;

// variable name
typedef struct AstVar {
    _AST_BASE_

    bool bound;

    u32 dbi; // de bruijn index

    Token* ident;
} AstVar;

#undef _AST_BASE_

Ast* parse_expr(TokenStream* ts);