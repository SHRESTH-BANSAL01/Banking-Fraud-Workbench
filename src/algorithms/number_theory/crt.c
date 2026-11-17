
#include <stdio.h>
#include <limits.h>
#include "crt.h"

/* Greatest Common Divisor */
static long long gcd_ll(long long a, long long b)
{
    while (b != 0) {
        long long temp = a % b;
        a = b;
        b = temp;
    }

    return a;
}

/* Safe modular addition for non-negative values */
static long long add_mod(long long a, long long b, long long m)
{
    if (a >= m - b) {
        return a - (m - b);
    }

    return a + b;
}

/* Modular multiplication without overflowing long long */
static long long multiply_mod(long long a, long long b, long long m)
{
    long long result = 0;

    while (b > 0) {
        if (b % 2 == 1) {
            result = add_mod(result, a, m);
        }

        b /= 2;

        if (b > 0) {
            a = add_mod(a, a, m);
        }
    }

    return result;
}

/* Modular inverse using repeated modular additions */
static long long modular_inverse(long long a, long long m)
{
    if (m == 1) {
        return 0;
    }

    long long result = 1;
    long long base = a % m;
    long long exponent = m - 2;

    /* This shortcut works only for prime moduli.
       General CRT requires the extended Euclidean algorithm. */
    (void)result;
    (void)base;
    (void)exponent;

    return -1;
}

/* Extended Euclidean modular inverse */
static long long inverse_general(long long a, long long m)
{
    long long t = 0;
    long long new_t = 1;
    long long r = m;
    long long new_r = a % m;

    while (new_r != 0) {
        long long q = r / new_r;

        /* Use modular coefficients to avoid large intermediate
           signed values while preserving the inverse modulo m. */
        long long q_mod = q % m;
        long long product = multiply_mod(q_mod, new_t, m);

        long long next_t =
            t >= product ? t - product : m - (product - t);

        long long next_r = r % new_r;

        t = new_t;
        new_t = next_t;

        r = new_r;
        new_r = next_r;
    }

    if (r != 1) {
        return -1;
    }

    return t;
}

void solve_crt(
    const long long remainders[],
    const long long moduli[],
    size_t n)
{
    if (remainders == NULL || moduli == NULL || n == 0) {
        printf("Invalid CRT input.\n");
        return;
    }

    long long product = 1;

    for (size_t i = 0; i < n; i++) {
        if (moduli[i] <= 1) {
            printf("All moduli must be greater than 1.\n");
            return;
        }

        for (size_t j = 0; j < i; j++) {
            if (gcd_ll(moduli[i], moduli[j]) != 1) {
                printf("Moduli must be pairwise coprime.\n");
                return;
            }
        }

        if (product > LLONG_MAX / moduli[i]) {
            printf("CRT modulus product exceeds integer range.\n");
            return;
        }

        product *= moduli[i];
    }

    long long solution = 0;

    for (size_t i = 0; i < n; i++) {
        long long m = moduli[i];
        long long remainder = remainders[i] % m;

        if (remainder < 0) {
            remainder += m;
        }

        long long partial = product / m;
        long long inverse = inverse_general(partial % m, m);

        if (inverse == -1) {
            printf("Modular inverse does not exist.\n");
            return;
        }

        long long term = multiply_mod(
            remainder, partial % product, product
        );

        term = multiply_mod(term, inverse, product);
        solution = add_mod(solution, term, product);
    }

    printf("\n============================================\n");
    printf("        CHINESE REMAINDER THEOREM\n");
    printf("============================================\n");

    printf("Given Congruences:\n");

    for (size_t i = 0; i < n; i++) {
        printf("x = %lld (mod %lld)\n",
               remainders[i], moduli[i]);
    }

    printf("\nSmallest Non-negative Solution: %lld\n",
           solution);

    printf("Solution Modulus: %lld\n", product);

    printf("\nVerification:\n");

    for (size_t i = 0; i < n; i++) {
        printf("%lld mod %lld = %lld\n",
               solution,
               moduli[i],
               solution % moduli[i]);
    }

    printf("============================================\n");
}
