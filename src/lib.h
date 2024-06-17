#pragma once

// useful lambda functions
// lifted from wikipedia lol

// group
#define G(...) "(" __VA_ARGS__ ")"

#define INC " (λn.λf.λx.f (n f x)) "
#define DEC " (λn.λf.λx.n (λg.λh.h (g f)) (λu.x) (λu.u)) "

#define ADD " (λm.λn.λf.λx.m f (n f x)) "
#define SUB " (λm.λn.n "DEC" m)"
#define MUL " (λm.λn.λf.m (n f)) "
#define DIV " ((λn.((λf.(λx.x x) (λx.f (x x))) (λc.λn.λm.λf.λx.(λd.(λn.n (λx.(λa.λb.b)) (λa.λb.a)) d ((λf.λx.x) f x) (f (c d m f x))) ((λm.λn.n (λn.λf.λx.n (λg.λh.h (g f)) (λu.x) (λu.u)) m) n m))) ((λn.λf.λx. f (n f x)) n))) "
#define POW " (λm.λn.n m) "

#define N0    " (λf.λx.x) "
#define N1    " (λf.λx.f x) "
#define N2    " (λf.λx.f (f x)) "
#define N3    " (λf.λx.f (f (f x))) "
#define N4    " (λf.λx.f (f (f (f x)))) "
#define N5    " (λf.λx.f (f (f (f (f x))))) "
#define N6    " (λf.λx.f (f (f (f (f (f x)))))) "
#define N7    " (λf.λx.f (f (f (f (f (f (f x))))))) "
#define N8    " (λf.λx.f (f (f (f (f (f (f (f x)))))))) "
#define N9    " (λf.λx.f (f (f (f (f (f (f (f (f x))))))))) "
#define N10   " (λf.λx.f (f (f (f (f (f (f (f (f (f x)))))))))) "
#define N100  " ("POW" "N10" "N2") "
#define N1000 " ("POW" "N10" "N3") "

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#define TRUE   " (λx.λy.x) "
#define FALSE  " (λx.λy.y) "
#define AND    " (λp.λq.p q p) "
#define OR     " (λp.λq.p p q) "
#define NOT    " (λp.p "FALSE" "TRUE") "
#define IFTHEN " (λp.λa.λb.p a b) "

#define ISZERO " (λn.n (λx."FALSE") "TRUE") "
#define LEQ    " (λm.λn."ISZERO" ("SUB" m n)) "

// combinators

#define I " (λx.x) "
#define S " (λx.λy.λz.x z (y z)) "
#define K " (λx.λy.x) "
#define B " (λx.λy.λz.x (y z)) "
#define C " (λx.λy.λz.x z y) "
#define W " (λx.λy.x y y) "
#define U " (λx.x x) "
#define Y " (λg.(λx.g (x x)) (λx.g (x x))) "