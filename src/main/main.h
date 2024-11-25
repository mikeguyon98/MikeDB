#ifndef MAIN_H
#define MAIN_H

#include <time.h>

#define TABLE_SIZE 1000

typedef struct kv_pair {
    char *key;
    char *val;
    time_t expiration;
    struct kv_pair *next;
} kv_pair_t;

extern kv_pair_t *hash_table[TABLE_SIZE];


#endif // MAIN_H