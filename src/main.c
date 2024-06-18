#define ORBIT_IMPLEMENTATION
#include "orbit.h"

#include "lambda.h"
#include "lib.h"

char* text = DIV N10 N5;

u64 expr_count = 0;

bool print_stats = false;
bool beta_recurse = false;


int main(int argc, char** argv) {


    // get string from command line args
    if (argc > 1) {

        for (int i = 1; i < argc; i++) {
            char* c = argv[i];
            if (strcmp(c, "-stats") == 0) {
                print_stats = true;
            } else
            if (strcmp(c, "-recurse") == 0) {
                beta_recurse = true;
            } else
            if (c[0] == '-') {
                printf("unrecognized option %s", c);
                return 1;
            }
            else {
                text = argv[i];
            }
        }
    }


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

    u64 iterations = 0;
    u64 b_redux = 0;
    u64 last_expr_count = expr_count;
    u64 b = 0;
    for (; (b = beta(&expr, beta_recurse)); iterations++) {
        b_redux += b;
        if (print_stats) {
            printf(" -> iter %llu, %llu β reductions so far, %llu nodes active (%+lld)\n", iterations, b_redux, expr_count, (i64)expr_count - (i64)last_expr_count);
            last_expr_count = expr_count;      
        } else {
            printf(" β  "); print_debruijn(expr); printf("\n");
        }
    }

    printf(" :: "); print_debruijn(expr); printf("\n");
    printf(" :: "); print_standard(expr); printf("\n");
    printf(" -- %llu iterations, %llu β-reductions, %llu nodes\n", iterations, b_redux, expr_count);

    alloca_deinit();
}

// TODO complete beta reductions in batches
// figure out how to do this without wasting work from beta reductions