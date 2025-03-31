#ifndef ENV_H
#define ENV_H

#include "types.h"

// Environment entry structure
typedef struct EnvEntry
{
    char *key;
    MalType *(*func)(MalType **);
    struct EnvEntry *next;
} EnvEntry;

// Environment structure
typedef struct
{
    EnvEntry *entries;
} Env;

Env *env_new();
void env_set(Env *env, const char *key, MalType *(*func)(MalType **));
MalType *(*env_get(Env *env, const char *key))(MalType **);
void env_free(Env *env);

#endif
