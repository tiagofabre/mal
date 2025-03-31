#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <string.h>

// Type enum for distinguishing between different Mal types
typedef enum {
    MAL_NIL,
    MAL_NUMBER,
    MAL_SYMBOL,
    MAL_LIST,
    // Future types can be added here (string, keyword, etc.)
} MalTypeEnum;

// Forward declaration of MalType
typedef struct MalType MalType;

// List structure to hold a list of Mal values
typedef struct {
    MalType **items;
    int count;
    int capacity;
} MalList;

// Symbol structure to hold a symbol name
typedef struct {
    char *value;
} MalSymbol;

// Main Mal type structure
struct MalType {
    MalTypeEnum type;
    union {
        long number;
        MalSymbol symbol;
        MalList list;
    } value;
};

// Functions for creating Mal types
MalType* mal_number_new(long num);
MalType* mal_symbol_new(const char* name);
MalType* mal_list_new();
void mal_list_add(MalType* list, MalType* item);
void mal_free(MalType* val);

#endif
