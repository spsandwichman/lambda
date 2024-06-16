#define ORBIT_IMPLEMENTATION
#include "orbit.h"

#include "lambda.h"

#define SUCC  "(λn.λf.λx.f (n f x)) "
#define PLUS  "(λm.λn.λf.λx.m f (n f x)) "

#define ONE   "(λx.λy.x y) "
#define TWO   "(λx.λy.x (x y)) "
#define THREE "(λx.λy.x (x (x y))) "

#define G(...) "(" __VA_ARGS__ ")" 

string text = str(
    PLUS ONE G( PLUS ONE G( PLUS ONE ONE ) )
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

    printf("=> "); print_debruijn(expr); printf("\n");
    //print_standard(expr); printf("\n\n");

    while (beta(&expr)) {
        printf("   "); print_debruijn(expr); printf("\n");
    }

    printf(":: ");
    print_standard(expr);
    printf("\n");


}