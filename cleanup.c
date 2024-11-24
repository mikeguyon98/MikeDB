#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "main.h"

#define CLEANUP_INTERVAL 60

// # 1 -> 2 -> 3 -> 4
 
void *cleanup_expired_keys(void *arg){
    while (1){
        sleep(CLEANUP_INTERVAL);
        time_t now = time(NULL);

        for (int i = 0; i < TABLE_SIZE; i++){
            kv_pair_t *pair = hash_table[i];
            kv_pair_t *prev = NULL;

            while (pair != NULL) {
                if (pair->expiration <= now){
                    printf("Deleting expired key: %s\n", pair->key);
                    if (prev == NULL){
                        hash_table[i] = pair->next;
                    } else {
                        prev->next = pair->next;
                    }
                    free(pair->key);
                    free(pair->val);
                    kv_pair_t *temp = pair;
                    pair = pair->next;
                    free(temp);
                } else {
                    prev = pair;
                    pair = pair->next;
                }
            }
        }
    }
    return NULL;
}