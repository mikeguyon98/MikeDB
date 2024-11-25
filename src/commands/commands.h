#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdbool.h>

unsigned int hash(const char *key);
void set(const char *key, const char *val, int ttl);
char *get(const char *key);
bool delete(const char *key);

#endif