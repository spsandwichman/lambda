#include "lambda.h"

bool is_special(char c) {
    return (c == '\\' || c == '.' || c == '(' || c == ')');
}

bool is_whitespace(char c) {
    return (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v');
}

void lex(TokenStream* ts, string text) {
    
    da_init(ts, 16);

    for (u64 cursor = 0; cursor < text.len; cursor++) {
        while (cursor < text.len && is_whitespace(text.raw[cursor])) cursor++;

        if (cursor >= text.len) break;

        switch (text.raw[cursor]) {
            
        case '\\': da_append(ts, ((Token){ &text.raw[cursor], 1, TOK_LAMBDA })); continue;
        case '.':  da_append(ts, ((Token){ &text.raw[cursor], 1, TOK_PERIOD })); continue;
        case '(':  da_append(ts, ((Token){ &text.raw[cursor], 1, TOK_OPEN_PAREN })); continue;
        case ')':  da_append(ts, ((Token){ &text.raw[cursor], 1, TOK_CLOSE_PAREN })); continue;
        case '\xCE':
            if (text.raw[cursor + 1] == '\xBB') {
                da_append(ts, ((Token){ &text.raw[cursor], 2, TOK_LAMBDA })); 
                cursor++;
                continue;
            }
        default:
            u64 len = 0;
            while (cursor + len < text.len) {
                if (is_whitespace(text.raw[cursor+len]) || (is_special(text.raw[cursor+len]))) break;
                len++;
            }
            da_append(ts, ((Token){ &text.raw[cursor], len, TOK_IDENT }));
            cursor += len - 1;
            continue;
        }
    }
    da_append(ts, ((Token){ &text.raw[text.len-1], 0, TOK_EOF}));
}

char* token_kind_strings[] = {
    [TOK_LAMBDA]        = "lambda",
    [TOK_PERIOD]        = ".",
    [TOK_OPEN_PAREN]    = "(",
    [TOK_CLOSE_PAREN]   = ")",
    [TOK_IDENT]         = "ident",
    [TOK_EOF]           = "EOF",
};