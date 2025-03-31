#ifndef READER_H
#define READER_H

#include "types.h"

// Reader structure to track tokens and position
typedef struct
{
    char **tokens;
    int position;
    int count;
} Reader;

// Reader functions
MalType *read_str(const char *str);
char **tokenize(const char *str, int *count);
MalType *read_form(Reader *reader);
MalType *read_list(Reader *reader);
MalType *read_atom(Reader *reader);

#endif
