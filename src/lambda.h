#pragma once

#include "orbit.h"

enum {
    TOK_LAMBDA, // backslash
    TOK_PERIOD, // .
    TOK_OPEN_PAREN, // (
    TOK_CLOSE_PAREN, // )
    TOK_IDENT,  // any series of unidentified characters
    TOK_EOF, // end of file
};

typedef struct Token {
    char* raw;
    u32 len;
    u8 kind;
} Token;

typedef struct TokenStream {
    Token* at;
    u64 len;
    u64 cap;

    u32 cursor;
} TokenStream;

void lex(TokenStream* ts, string text);

extern char* token_kind_strings[];
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

enum {
    EXPR_LAM, // lambda term
    EXPR_APP, // application
    EXPR_VAR, // variable
};

typedef struct Expr Expr;

// De Bruijn term
typedef struct Expr {
    // Expr* parent;
    u8 kind;
    union {
        Expr* lam; // body of lambda
        struct {
            Expr* func;
            Expr* input;
        } app;
        struct {
            u64 index;
            bool bound;
        } var;
    };
} Expr;

Ast* new_ast(u8 kind);

Ast* parse_expr(TokenStream* ts);
Ast* parse_term(TokenStream* ts);
Ast* parse_lambda(TokenStream* ts);

Expr* parse(TokenStream* ts);

void print_debruijn(Expr* expr);
void print_standard(Expr* expr);

bool beta(Expr** expr);
bool is_beta_reducible(Expr* expr);