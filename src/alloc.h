#pragma once

#include "lambda.h"

#define ALLOC_POOL_LENGTH (1ull<<20ull)

typedef struct PoolFreeBlock PoolFreeBlock;
typedef struct ExprPool ExprPool;
typedef struct ExprAllocator ExprAllocator;

typedef struct PoolFreeBlock {
    PoolFreeBlock* next;
} PoolFreeBlock;

static_assert(sizeof(PoolFreeBlock) <= sizeof(Expr));

typedef struct ExprPool {
    Expr* pool;

    PoolFreeBlock* head;
} ExprPool;

// holds multiple pools
typedef struct ExprAllocator {
    ExprPool* at;
    size_t len;
    size_t cap;
    size_t last_used;

    u64 max_nodes;
    u64 current_nodes;
} ExprAllocator;

void alloca_init();
void alloca_deinit();

Expr* alloca_new();
void alloca_delete(Expr* e);

extern ExprAllocator nodes;