#define ORBIT_IMPLEMENTATION
#include "orbit.h"

#include "lambda.h"
#include "lib.h"

char* text = G(DIV N100 N10);

u64 expr_count = 0;

int main() {

    alloca_init();


    printf(" :: %s\n", text);

    TokenStream ts = {0};
    lex(&ts, str(text));
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
        // printf(" β  "); print_debruijn(expr); printf("\n");
        // printf(" -> %llu, %llu nodes\n", i, expr_count);        
    }

    printf(" :: "); print_debruijn(expr); printf("\n");
    printf(" -- %llu β-reductions, %llu nodes\n", i, expr_count);

    alloca_deinit();
}

// TODO complete beta reductions in batches
// figure out how to do this without wasting work from beta reductions