#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#define TABLE_SIZE 1000
#define INPUT_BUFFER_SIZE 256

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
    time_t now = time(NULL);

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
    char input[INPUT_BUFFER_SIZE];

    while (1) {
        printf("\n======MikeDB======\n");
        if (fgets(input, INPUT_BUFFER_SIZE, stdin) == NULL) {
            // Handle EOF (Ctrl+D)
            break;
        }
        input[strcspn(input, "\n")] = '\0';

        char *command = strtok(input, " ");
        if (command == NULL) {
            continue;
        }

        for (char *p = command; *p; ++p) {
            *p = toupper(*p);
        }

        char *arg1 = strtok(NULL, " ");
        char *arg2 = strtok(NULL, " ");
        char *arg3 = strtok(NULL, " ");

        if (strcmp(command, "GET") == 0){
            if (arg1 == NULL) {
                printf("usage: GET <key>\n");
                continue;
            }
            char *value = get(arg1);
            if (value != NULL) {
                printf("%s\n", value);
            } else {
                printf("(nil)\n");
            }
        } else if (strcmp(command, "SET") == 0){
            if (arg1 == NULL || arg2 == NULL) {
                printf("usage: SET <key> <value> <exipiration (optional)>\n");
                continue;
            }
            int expiration = arg3 != NULL ? atoi(arg3) : 0;
            set(arg1, arg2, expiration);
            printf("INSERTED\n");
        } else if (strcmp(command, "DELETE") == 0){
            if (arg1 == NULL){
                printf("usage: DELETE <key>\n");
                continue;
            }
            bool deleted = delete(arg1);
            if (deleted) {
                printf("DELETED\n");
            } else {
                printf("NOT FOUND\n");
            }
        } else if (strcmp(command, "EXIT") == 0){
            break;
        } else {
            printf("Invalid input\n");
        }
    }
    printf("EXITING.....\n");
    return 0;
}
