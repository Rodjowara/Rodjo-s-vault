#ifndef CUSTOM_HASH_H
#define CUSTOM_HASH_H

#include <stdint.h>

// Convenient type for hash values
typedef uint64_t hash_t;

// Return the hash of an empty tree
static inline hash_t hash_empty(void) {
    return 1469598103934665603ULL;
}

// The hash combine function
static inline hash_t hash_combine(hash_t left, float key, hash_t right) {
    const uint64_t PRIME = 1099511628211ULL;
    hash_t h = hash_empty();

    h ^= left;
    h *= PRIME;

    union {
        float f;
        uint32_t u;
    } key_bits;
    key_bits.f = key;

    h ^= key_bits.u;
    h *= PRIME;

    h ^= right;
    h *= PRIME;

    return h;
}

// Assignment function prototypes
hash_t tree_hash_seq(node_t *root);
hash_t tree_hash_parallel(node_t *root, int depth);

#endif // CUSTOM_HASH_H