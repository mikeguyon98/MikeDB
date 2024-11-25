#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include "../main/main.h"

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
                    hash_table[slot] = pair->next;
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

bool delete(const char *key){
    unsigned int slot = hash(key);
    kv_pair_t *pair = hash_table[slot];
    kv_pair_t *prev = NULL;
    
    while (pair != NULL){
        if(strcmp(key, pair->key)==0){
            if(prev==NULL){
                hash_table[slot] = pair->next;
            } else {
                prev->next = pair->next;
            }
            free(pair->key);
            free(pair->val);
            free(pair);
            return true;
        }
        prev = pair;
        pair = pair->next;
    }
    return false;
}