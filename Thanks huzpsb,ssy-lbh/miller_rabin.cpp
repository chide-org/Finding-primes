#include "number_theory.h"
#include "big_num.h"

bool miller_rabin(uint32_t n){
    if (n < 3 || (n & 1) == 0)
        return n == 2;
    uint32_t d = n - 1, s = 0;
    static constexpr uint32_t ud[] = {2, 7, 61};
    while ((d & 1) == 0)
        d >>= 1, s++;
    for (uint32_t a : ud){
        uint32_t x = pow(a, d, n);
        if (x == 1 || x == n - 1 || x == 0)
            continue;
        for (uint32_t i = 0; i < s; i++){
            x = (uint64_t)x * x % n;
            if (x == n - 1 && i != s - 1)
                goto next;
            if (x == 1) return false;
        }
        if (x != 1)
            return false;
        next:;
    }
    return true;
}

bool miller_rabin(uint64_t n){
    if (n < 3 || (n & 1) == 0)
        return n == 2;
    uint64_t d = n - 1;
    int s = 0;
    static constexpr uint64_t ud[] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    while ((d & 1) == 0)
        d >>= 1, s++;
    for (uint64_t a : ud){
        uint64_t x = pow(a, d, n);
        if (x == 1 || x == n - 1 || x == 0)
            continue;
        for (int i = 0; i < s; i++){
            x = (uint64_t)x * x % n;
            if (x == n - 1 && i != s - 1)
                goto next;
            if (x == 1) return false;
        }
        if (x != 1)
            return false;
        next:;
    }
    return true;
}

bool miller_rabin(BigInt n){
    if (n < 3 || (n & 1) == 0)
        return n == 2;
    BigInt d = n - 1;
    uint64_t s = 0;
    while ((d & 1) == 0)
        d >>= 1, s++;
    for (int i = 0; i < 20; i++){
        BigInt x = pow(randmod(n), d, n);
        if (x <= 1 || x + 1 == n)
            continue;
        for (uint64_t i = 0; i < s; i++){
            x = mul(x, x, n);
            if (x + 1 == n && i != s - 1)
                goto next;
            if (x == 1) return false;
        }
        if (x != 1)
            return false;
        next:;
    }
    return true;
}

bool miller_rabin(BigInt n, int reps){
    return mpz_millerrabin(n.n, reps);
}