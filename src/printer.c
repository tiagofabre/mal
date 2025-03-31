#include "printer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Print Mal data structure as a string
char* pr_str(MalType* val, int print_readably) {
    if (!val) return strdup("nil");
    
    switch (val->type) {
        case MAL_NIL:
            return strdup("nil");
            
        case MAL_NUMBER: {
            char buf[32];
            sprintf(buf, "%ld", val->value.number);
            return strdup(buf);
        }
            
        case MAL_SYMBOL:
            return strdup(val->value.symbol.value);
            
        case MAL_LIST: {
            int capacity = 256;
            char* result = malloc(capacity);
            strcpy(result, "(");
            int len = 1;
            
            for (int i = 0; i < val->value.list.count; i++) {
                char* elem_str = pr_str(val->value.list.items[i], print_readably);
                
                // Ensure we have enough space in result buffer
                int needed = len + strlen(elem_str) + 2; // +2 for space and potential closing paren
                if (needed > capacity) {
                    capacity = needed * 2;
                    result = realloc(result, capacity);
                }
                
                // Add space between elements
                if (i > 0) {
                    result[len++] = ' ';
                    result[len] = '\0';
                }
                
                // Append element string
                strcat(result, elem_str);
                len += strlen(elem_str);
                free(elem_str);
            }
            
            // Add closing parenthesis
            result[len++] = ')';
            result[len] = '\0';
            
            return result;
        }
            
        default:
            return strdup("unknown");
    }
}
