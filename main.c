#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include "cleanup.h"
#include "commands.h"

#define TABLE_SIZE 1000
#define INPUT_BUFFER_SIZE 256

typedef struct kv_pair {
    char *key;
    char *val;
    time_t expiration;
    struct kv_pair *next;
} kv_pair_t;

kv_pair_t *hash_table[TABLE_SIZE];

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

    pthread_t cleanup_thread;
    if (pthread_create(&cleanup_thread, NULL, cleanup_expired_keys, NULL) != 0){
        fprintf(stderr, "Error creating cleaup thread\n");
        return EXIT_FAILURE;
    }

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
