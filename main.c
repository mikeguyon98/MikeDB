#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TABLE_SIZE 1000

typedef struct kv_pair {
    char *key;
    char *val;
    time_t expiration;
    struct kv_pair *next;
} kv_pair_t;

kv_pair_t *hash_table[TABLE_SIZE];

unsigned int hash(const char *key){
    unsigned long int val = 0;
    const unsigned char prime = 31;

    while (*key != '\0') {
        val = val * prime + *key++;
    }
    return val % TABLE_SIZE;
}

void set(const char *key, const char *val, int ttl){
    unsigned int hashed_key = hash(key);
    kv_pair_t *pair = hash_table[hashed_key];
    time_t now = time(NULL);

    while (pair != NULL){
        if (strcmp(pair->key, key) == 0) {
            free(pair->val);
            pair->val = strdup(val);
            pair->expiration = ttl > 0 ? now + ttl : 0;
            return;
        }
        pair = pair -> next;
    }

    kv_pair_t *new_pair = malloc(sizeof(kv_pair_t));
    if (new_pair == NULL){
        fprintf(stderr, "Memory allocation failed \n");
        exit(EXIT_FAILURE);
    }

    new_pair->key = strdup(key);
    new_pair->val = strdup(val);
    new_pair->expiration = ttl > 0 ? ttl + now : 0;
    new_pair->next = hash_table[hashed_key];
    hash_table[hashed_key] = new_pair;
}

char *get(const char *key){
    unsigned int slot = hash(key);
    kv_pair_t *pair = hash_table[slot];
    kv_pair_t *prev = NULL;
    time_t now = time(NULL);

    while (pair != NULL){
        if (strcmp(pair->key, key) == 0){
            if(pair->expiration > now || pair->expiration == 0){
                return pair->val;
            } else {
                if (prev == NULL){
                    pair = pair->next;
                } else {
                    prev->next = pair -> next;
                }
                free(pair->key);
                free(pair->val);
                free(pair);
                return NULL;
            }
        }
        prev = pair;
        pair = pair->next;
    }
    return NULL;
}

void print_hash_table(){
    size_t length = sizeof(hash_table) / sizeof(hash_table[0]);
    for (int i = 0 ; i < length ; i++) {
        kv_pair_t *pair = hash_table[i];
        while (pair != NULL) {
            printf("slot: %d \t | key: %10s \t | val: %s \n", i, pair->key, pair->val);
            pair = pair->next;
        }
    }
}

int main() {
    kv_pair_t pair_1 = {
        .key = strdup("exampleKey"),
        .val = strdup("exampleValue"),
        .next = NULL
    };

    kv_pair_t pair_2 = {
        .key = strdup("123"),
        .val = strdup("Mike"),
        .next = NULL
    };
    
    kv_pair_t pair_3 = {
        .key = strdup("345"),
        .val = strdup("Dan"),
        .next = NULL
    };
    
    set(pair_1.key, pair_1.val, 2000);
    set(pair_2.key, pair_2.val, 2000);
    set(pair_3.key, pair_3.val, 1);


    char *key_1 = "123";
    char *key_2 = "345";
    char *key_3 = "exampleKey";
    char *key_4 = "missing";
    char *val_1 = get(key_1);
    char *val_2 = get(key_2);
    char *val_3 = get(key_3);
    char *val_4 = get(key_4);
    printf("======KEY-VALS======\n");
    printf("%10s : %10s\n", key_1, val_1);
    printf("%10s : %10s\n", key_2, val_2);
    printf("%10s : %10s\n", key_3, val_3);
    printf("%10s : %10s\n", key_4, val_4);

    printf("\n\n======TABLE=====\n\n");
    print_hash_table();

    free(pair_1.key);
    free(pair_1.val);
    free(pair_2.key);
    free(pair_2.val);
    free(pair_3.key);
    free(pair_3.val);

    return 0;
}
