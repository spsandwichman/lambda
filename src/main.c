#define ORBIT_IMPLEMENTATION
#include "orbit.h"

#include "lambda.h"

string text = constr("(\\y.(\\x.x)) z");

int main() {
    printf("::: "str_fmt" => \n", str_arg(text));

    TokenStream ts = {0};

    lex(&ts, text);

    Ast* expr = parse(&ts);

    de_bruijn(expr);
    print_tree(expr);

    eval(expr);
}