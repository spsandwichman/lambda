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

Expr* parse(TokenStream* ts);

void print_debruijn(Expr* expr);
void print_standard(Expr* expr);

bool beta(Expr** expr);
bool is_beta_reducible(Expr* expr);