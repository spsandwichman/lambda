#define ORBIT_IMPLEMENTATION
#include "orbit.h"

#include "lambda.h"

#define SUCC  "(λn.λf.λx.f (n f x)) "
#define PLUS  "(λm.λn.λf.λx.m f (n f x)) "
#define MULT  "(λm.λn.λf.m (n f))"

#define ZERO  "(λf.λx.x)"
#define ONE   "(λf.λx.f x) "
#define TWO   "(λf.λx.f (f x)) "
#define THREE "(λf.λx.f (f (f x))) "

#define G(...) "(" __VA_ARGS__ ")" 

string text = str(
    MULT TWO TWO
);

int main() {
    printf("\n:: "str_fmt"\n", str_arg(text));

    TokenStream ts = {0};

    lex(&ts, text);

    Expr* expr = parse(&ts);

    if (expr == NULL) {
        printf("failed to parse lambda expression");
        return 1;
    }

    printf(" = "); print_debruijn(expr); printf("\n");
    //print_standard(expr); printf("\n\n");

    while (beta(&expr)) {
        printf(" = "); print_debruijn(expr); printf("\n");
    }

    printf(":: ");
    print_standard(expr);
    printf("\n");


}