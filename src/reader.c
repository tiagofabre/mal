#include "reader.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>

// Simple tokenizer (a full implementation would use the regex from the spec)
char** tokenize(const char* str, int* count) {
    char* buf = strdup(str);
    int capacity = 10;
    char** tokens = malloc(capacity * sizeof(char*));
    *count = 0;
    
    // Skip leading whitespace
    char* p = buf;
    while (*p && isspace(*p)) p++;
    
    while (*p) {
        if (*p == '(' || *p == ')') {
            // Handle parentheses as individual tokens
            if (*count >= capacity) {
                capacity *= 2;
                tokens = realloc(tokens, capacity * sizeof(char*));
            }
            char* token = malloc(2);
            token[0] = *p;
            token[1] = '\0';
            tokens[(*count)++] = token;
            p++;
        } else if (isspace(*p)) {
            // Skip whitespace
            p++;
        } else {
            // Read a symbol or number
            char* start = p;
            while (*p && !isspace(*p) && *p != '(' && *p != ')') p++;
            
            if (*count >= capacity) {
                capacity *= 2;
                tokens = realloc(tokens, capacity * sizeof(char*));
            }
            
            int len = p - start;
            char* token = malloc(len + 1);
            strncpy(token, start, len);
            token[len] = '\0';
            tokens[(*count)++] = token;
        }
        
        // Skip any whitespace
        while (*p && isspace(*p)) p++;
    }
    
    free(buf);
    return tokens;
}

// Create a new Reader with tokens
Reader* create_reader(char** tokens, int count) {
    Reader* reader = malloc(sizeof(Reader));
    reader->tokens = tokens;
    reader->position = 0;
    reader->count = count;
    return reader;
}

// Free the Reader and its tokens
void free_reader(Reader* reader) {
    for (int i = 0; i < reader->count; i++) {
        free(reader->tokens[i]);
    }
    free(reader->tokens);
    free(reader);
}

// Peek at the current token
char* peek(Reader* reader) {
    if (reader->position >= reader->count) {
        return NULL;
    }
    return reader->tokens[reader->position];
}

// Get the current token and advance
char* next(Reader* reader) {
    if (reader->position >= reader->count) {
        return NULL;
    }
    return reader->tokens[reader->position++];
}

// Read a string into a Mal data structure
MalType* read_str(const char* str) {
    int count;
    char** tokens = tokenize(str, &count);
    if (count == 0) {
        free(tokens);
        return NULL;
    }
    
    Reader* reader = create_reader(tokens, count);
    MalType* result = read_form(reader);
    free_reader(reader);
    return result;
}

// Read a form based on the first token
MalType* read_form(Reader* reader) {
    char* token = peek(reader);
    if (!token) return NULL;
    
    if (strcmp(token, "(") == 0) {
        return read_list(reader);
    } else {
        return read_atom(reader);
    }
}

// Read a list
MalType* read_list(Reader* reader) {
    MalType* list = mal_list_new();
    
    // Skip opening paren
    next(reader);
    
    char* token;
    while ((token = peek(reader)) != NULL && strcmp(token, ")") != 0) {
        mal_list_add(list, read_form(reader));
    }
    
    if (!token || strcmp(token, ")") != 0) {
        // Unbalanced parenthesis
        mal_free(list);
        return NULL;
    }
    
    // Skip closing paren
    next(reader);
    
    return list;
}

// Read an atom (number or symbol)
MalType* read_atom(Reader* reader) {
    char* token = next(reader);
    if (!token) return NULL;
    
    // Check if token is a number
    char* endptr;
    long num = strtol(token, &endptr, 10);
    
    if (*endptr == '\0') {
        // It's a number
        return mal_number_new(num);
    } else {
        // It's a symbol
        return mal_symbol_new(token);
    }
}
