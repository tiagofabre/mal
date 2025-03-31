#include "env.h"
#include <stdlib.h>
#include <string.h>

Env *env_new()
{
    Env *env = malloc(sizeof(Env));
    env->entries = NULL;
    return env;
}

void env_set(Env *env, const char *key, MalType *(*func)(MalType **))
{
    EnvEntry *entry = env->entries;
    while (entry)
    {
        if (strcmp(entry->key, key) == 0)
        {
            entry->func = func;
            return;
        }
        entry = entry->next;
    }

    entry = malloc(sizeof(EnvEntry));
    entry->key = strdup(key);
    entry->func = func;
    entry->next = env->entries;
    env->entries = entry;
}

MalType *(*env_get(Env *env, const char *key))(MalType **)
{
    EnvEntry *entry = env->entries;
    while (entry)
    {
        if (strcmp(entry->key, key) == 0)
        {
            return entry->func;
        }
        entry = entry->next;
    }
    return NULL;
}

void env_free(Env *env)
{
    EnvEntry *entry = env->entries;
    while (entry)
    {
        EnvEntry *next = entry->next;
        free(entry->key);
        free(entry);
        entry = next;
    }
    free(env);
}
