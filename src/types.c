#include "types.h"
#include <stdio.h>

MalType *mal_number_new(long num)
{
    MalType *val = malloc(sizeof(MalType));
    val->type = MAL_NUMBER;
    val->value.number = num;
    return val;
}

MalType *mal_symbol_new(const char *name)
{
    MalType *val = malloc(sizeof(MalType));
    val->type = MAL_SYMBOL;
    val->value.symbol.value = strdup(name);
    return val;
}

MalType *mal_list_new()
{
    MalType *val = malloc(sizeof(MalType));
    val->type = MAL_LIST;
    val->value.list.items = NULL;
    val->value.list.count = 0;
    val->value.list.capacity = 0;
    return val;
}

void mal_list_add(MalType *list, MalType *item)
{
    if (list->type != MAL_LIST)
    {
        return;
    }

    if (list->value.list.count >= list->value.list.capacity)
    {
        int new_capacity = list->value.list.capacity == 0 ? 2 : list->value.list.capacity * 2;
        list->value.list.items = realloc(list->value.list.items, new_capacity * sizeof(MalType *));
        list->value.list.capacity = new_capacity;
    }

    list->value.list.items[list->value.list.count++] = item;
}

void mal_free(MalType *val)
{
    if (!val)
        return;

    printf("Freeing type: %d at address %p\n", val->type, val);

    if (val->type == MAL_SYMBOL)
    {
        free(val->value.symbol.value);
    }
    else if (val->type == MAL_LIST)
    {
        for (int i = 0; i < val->value.list.count; i++)
        {
            mal_free(val->value.list.items[i]);
        }
        free(val->value.list.items);
    }

    free(val);
}
