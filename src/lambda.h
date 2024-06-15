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
    u32 len;
    u32 cap;

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

    u32 dbi; // de bruijn index

    Token* ident;
    AstLambda* bind; // NULL if is a free variable
} AstVar;

Ast* new_ast(u8 kind);

Ast* parse(TokenStream* ts);
Ast* parse_expr(TokenStream* ts);
Ast* parse_term(TokenStream* ts);
Ast* parse_lambda(TokenStream* ts);

void print_tree(Ast* expr);

void de_bruijn(Ast* expr);

Ast* eval(Ast* expr);