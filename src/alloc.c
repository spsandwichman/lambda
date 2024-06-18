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

ExprAllocator alloc = {0};

void alloca_init() {
    da_init(&alloc, 4);
    pool_init(&alloc.at[0]);
}

void alloca_deinit() {
    for (size_t i = 0; i < alloc.len; i++) {
        free(alloc.at[i].pool);
    }
    da_destroy(&alloc);
    alloc = (ExprAllocator){0};
}

Expr* alloca_new() {
    alloc.current_nodes++;
    for (size_t i = 0; i < alloc.len; i++) {
        Expr* n = pool_alloc(&alloc.at[i]);
        if (n != NULL) return n;
    }
    // we ran out of space and need to make a new pool
    ExprPool np = {0};
    pool_init(&np);
    Expr* n = pool_alloc(&np);
    if (n == NULL) CRASH("failed to allocate on new pool");

    da_append(&alloc, np);
    alloc.max_nodes = max(alloc.current_nodes, alloc.max_nodes);
    return n;
}

void alloca_delete(Expr* e) {
    alloc.current_nodes--;
    for (size_t i = 0; i < alloc.len; i++) {
        if (pool_free(&alloc.at[i], e)) {
            return;
        }
    }
    CRASH("failed to free expression");
}