#define ORBIT_IMPLEMENTATION
#include "orbit.h"

#include "lambda.h"

#define SUC   "(λn.λf.λx.f (n f x)) "
#define ADD   "(λm.λn.λf.λx.m f (n f x)) "
#define MUL   "(λm.λn.λf.m (n f))"

#define ZERO  "(λf.λx.x)"
#define ONE   "(λf.λx.f x) "
#define TWO   "(λf.λx.f (f x)) "
#define THREE "(λf.λx.f (f (f x))) "
#define FOUR  "(λf.λx.f (f (f (f x)))) "

#define G(...) "(" __VA_ARGS__ ")" 

string text = str(
    // G(G(G(FOUR MUL) TWO G(G(G(G(FOUR MUL) TWO TWO) G(ADD TWO)) TWO)) G(ADD TWO)) TWO
    G(MUL FOUR G(MUL TWO G(MUL TWO G(MUL TWO G(MUL TWO G(MUL TWO G(MUL TWO G(MUL TWO ZERO))))))))
);

u64 expr_count = 0;

int main() {
    printf(" :: "str_fmt"\n", str_arg(text));

    TokenStream ts = {0};

    lex(&ts, text);

    Expr* expr = parse(&ts);

    if (expr == NULL) {
        printf("failed to parse lambda expression");
        return 1;
    }

    // print_standard is super buggy, not recommended
    // printf(" :: "); print_standard(expr); printf("\n");
    printf(" :: "); print_debruijn(expr); printf("\n");


    u64 i = 0;
    for (; beta(&expr); i++) {
        printf(" -> "); print_debruijn(expr); printf("\n");        
    }

    printf(" :: "); print_debruijn(expr); printf("\n");
    printf("    final: %llu β-reductions, %llu nodes\n", i, expr_count);
}