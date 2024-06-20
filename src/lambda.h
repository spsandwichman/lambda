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
    u8 kind;
    union {
        Expr* lam; // body of lambda
        struct {
            Expr* func;
            Expr* input;
        } app;
        struct {
            u64 index;
        } var;
    };
} Expr;

static_assert(sizeof(void*) == sizeof(u64));

typedef struct Term {
    u64 kind : 2;
    u64 lam : 31;
    u64 in  : 31;
} Term;

typedef u64 TermIndex;

static_assert(sizeof(Term) == sizeof(u64));

#define MAX_TERM_COUNT 0x7FFFFFFF

// #define get_kind(term)       ((term) & 0b11)
// #define set_kind(term, kind) (((term) & ~(0b11ll)) | (kind & (0b11ll)))

// #define get_lam(term)      (((term) & 0x7FFFFFFFll) >> 2)
// #define set_lam(term, lam) (((term) & ~(0x1FFFFFFFCll)) | (((lam) & (0x7FFFFFFFll)) << 2))

// #define get_app_fn(term) (((term) & 0x7FFFFFFFll) >> 2)
// #define get_app_in(term) ((((term) >> 33) & 0x7FFFFFFFll)

// #define set_app_fn(term, fn) (((term) & ~(0x1FFFFFFFCll)) | (((fn) & (0x7FFFFFFFll)) << 2))
// #define set_app_in(term, in) (((term) & (0xFFFFFFFE00000000ll)) | (((fn) & (0x7FFFFFFFll)) << 33))

Expr* parse(TokenStream* ts);

void print_debruijn(Expr* expr);
void print_standard(Expr* expr);

u64 beta(Expr** expr, bool recurse);

#include "alloc.h"