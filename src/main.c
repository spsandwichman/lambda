#define ORBIT_IMPLEMENTATION
#include "orbit.h"

#include "lambda.h"
#include "lib.h"

// char* text = G(POW G(POW N1000 N1000) N1000);
char* text = DIV G(ADD G(MUL N2 N100) N2) N7;

enum {
    PRINT_DB,
    PRINT_STATS,
    PRINT_NONE,
};

i64  min_diff = 0;
u8   print = PRINT_DB;
bool beta_recurse = false;
char* separator = NULL;

int main(int argc, char** argv) {

    // get string from command line args
    if (argc > 1) {

        for (int i = 1; i < argc; i++) {
            char* c = argv[i];
            if (strcmp(c, "-print-stats") == 0) {
                print = PRINT_STATS;
            } else
            if (strcmp(c, "-print-none") == 0) {
                print = PRINT_NONE;
            } else
            if (strcmp(c, "-recurse") == 0) {
                beta_recurse = true;
            } else
            if (strcmp(c, "-min-diff") == 0) {
                i++;
                min_diff = atoll(argv[i]);
            } else
            if (strcmp(c, "-sep") == 0) {
                i++;
                separator = argv[i];
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
    u64 last_expr_count = nodes.current_nodes;
    u64 b = 0;
    for (; (b = beta(&expr, beta_recurse)); iterations++) {
        b_redux += b;

        switch (print) {
        case PRINT_DB:
            printf(" β  "); print_debruijn(expr); printf("\n");
            if (separator != NULL) printf("%s", separator);
            break;
        case PRINT_STATS:
            i64 diff = (i64)nodes.current_nodes - (i64)last_expr_count;
            if (min_diff == 0 || !(-min_diff < diff && diff < min_diff)) {
                printf(" -> iter %llu, %llu β reductions so far, %llu (%+lld) nodes active (%llu/%llu KB) \n", 
                    iterations + 1, b_redux, nodes.current_nodes, diff, nodes.current_nodes * sizeof(Expr) / 1000, nodes.len * ALLOC_POOL_LENGTH * sizeof(Expr) / 1000);
            }
            if (separator != NULL) printf("%s", separator);
            last_expr_count = nodes.current_nodes;
            break;
        default:
        }
    }

    printf(" :: "); print_debruijn(expr); printf("\n");
    printf(" :: "); print_standard(expr); printf("\n");
    printf(" -- %llu iterations, %llu β-reductions, %llu final nodes (%llu max)\n", iterations, b_redux, nodes.current_nodes, nodes.max_nodes);

    alloca_deinit();
}