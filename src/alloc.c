#include "alloc.h"

// implement a pool allocator for these nodes, since they're all the same size
// mostly copied from GingerBills

void pool_init(ExprPool* p);
Expr* pool_alloc(ExprPool* p);
void pool_free_all(ExprPool* p);
bool pool_free(ExprPool *p, Expr *ptr);

void pool_init(ExprPool* p) {
    p->pool = calloc(ALLOC_POOL_LENGTH, sizeof(Expr));
    p->head = NULL;

    pool_free_all(p);
}

Expr* pool_alloc(ExprPool* p) {
    PoolFreeBlock* node = p->head;

    if (node == NULL) return NULL;

    p->head = p->head->next;

    return (Expr*)node;
}

// returns if in range or not
bool pool_free(ExprPool* p, Expr* ptr) {
    if (ptr == NULL) return false;

    Expr* start = p->pool;
    Expr* end   = &p->pool[ALLOC_POOL_LENGTH];
    
    if (!(start <= ptr && ptr < end)) return false;

    PoolFreeBlock* node = (PoolFreeBlock*) ptr;
    node->next = p->head;
    p->head = node;

    return true;
}

void pool_free_all(ExprPool* p) {
    for (size_t i = 0; i < ALLOC_POOL_LENGTH; i++) {
        PoolFreeBlock* ptr = (PoolFreeBlock*)&p->pool[i];
        ptr->next = p->head;
        p->head = ptr;
    }
}

static ExprAllocator allocator = {0};

void alloca_init() {
    da_init(&allocator, 4);
    pool_init(&allocator.at[0]);
}

void alloca_deinit() {
    for (size_t i = 0; i < allocator.len; i++) {
        free(allocator.at[i].pool);
    }
    da_destroy(&allocator);
    allocator = (ExprAllocator){0};
}

Expr* alloca_new() {
    expr_count++;
    for (size_t i = 0; i < allocator.len; i++) {
        Expr* n = pool_alloc(&allocator.at[i]);
        if (n != NULL) return n;
    }
    // we ran out of space and need to make a new pool
    ExprPool np = {0};
    pool_init(&np);
    Expr* n = pool_alloc(&np);
    if (n == NULL) CRASH("failed to allocate on new pool");

    da_append(&allocator, np);
    return n;
}

void alloca_delete(Expr* e) {
    expr_count--;
    for (size_t i = 0; i < allocator.len; i++) {
        if (pool_free(&allocator.at[i], e)) {
            return;
        }
    }
    CRASH("failed to free expression");
}