#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "reader.h"
#include "printer.h"
#include "env.h"

// Function prototypes
MalType *READ(char *input);
MalType *EVAL(MalType *ast, Env *env);
char *PRINT(MalType *val);
char *rep(char *input, Env *env);

// Core arithmetic functions
MalType *add_func(MalType **args)
{
    if (args[0]->type == MAL_NUMBER && args[1]->type == MAL_NUMBER)
    {
        // Create a new value without modifying the originals
        return mal_number_new(args[0]->value.number + args[1]->value.number);
    }
    return NULL;
}

MalType *sub_func(MalType **args)
{
    if (args[0]->type == MAL_NUMBER && args[1]->type == MAL_NUMBER)
    {
        return mal_number_new(args[0]->value.number - args[1]->value.number);
    }
    return NULL;
}

MalType *mul_func(MalType **args)
{
    if (args[0]->type == MAL_NUMBER && args[1]->type == MAL_NUMBER)
    {
        return mal_number_new(args[0]->value.number * args[1]->value.number);
    }
    return NULL;
}

MalType *div_func(MalType **args)
{
    if (args[0]->type == MAL_NUMBER && args[1]->type == MAL_NUMBER && args[1]->value.number != 0)
    {
        return mal_number_new(args[0]->value.number / args[1]->value.number);
    }
    return NULL;
}

// READ: Parse the input string into a Mal data structure
MalType *READ(char *input)
{
    return read_str(input);
}

// EVAL: Evaluate the AST
MalType *EVAL(MalType *ast, Env *env)
{
    if (!ast)
        return NULL;

    // Handle non-list types
    if (ast->type != MAL_LIST)
    {
        // Evaluate symbols - look them up in the environment
        if (ast->type == MAL_SYMBOL)
        {
            MalType *(*func)(MalType **) = env_get(env, ast->value.symbol.value);
            if (!func)
            {
                fprintf(stderr, "Symbol not found: %s\n", ast->value.symbol.value);
                return NULL;
            }
            // Return a clone of the symbol, since we look up functions during list evaluation
            return mal_symbol_new(ast->value.symbol.value);
        }
        // For non-lists and non-symbols, just return the ast unchanged
        return ast;
    }

    // Empty list case
    if (ast->value.list.count == 0)
    {
        return ast; // Return empty list as is
    }

    // Evaluate lists - apply the first element to the rest
    // First, evaluate all elements in the list
    MalType *evaluated_list = mal_list_new();
    for (int i = 0; i < ast->value.list.count; i++)
    {
        MalType *eval_result = EVAL(ast->value.list.items[i], env);
        if (!eval_result)
        {
            mal_free(evaluated_list);
            return NULL;
        }
        mal_list_add(evaluated_list, eval_result);
    }

    // Get the first element (should be a function symbol)
    MalType *first = evaluated_list->value.list.items[0];
    if (first->type == MAL_SYMBOL)
    {
        // Look up the function in the environment
        MalType *(*func)(MalType **) = env_get(env, first->value.symbol.value);
        if (!func)
        {
            fprintf(stderr, "Function not found: %s\n", first->value.symbol.value);
            mal_free(evaluated_list);
            return NULL;
        }

        // Extract arguments and call the function
        MalType **args = NULL;
        int arg_count = evaluated_list->value.list.count - 1;
        if (arg_count > 0)
        {
            args = malloc(arg_count * sizeof(MalType *));
            for (int i = 1; i < evaluated_list->value.list.count; i++)
            {
                args[i - 1] = evaluated_list->value.list.items[i];
            }
        }

        MalType *result = func(args);
        // free(args);
        // mal_free(evaluated_list); // Free the evaluated list and its elements
        return result;
    }

    // If the first element is not a symbol, just return the evaluated list
    return evaluated_list;
}

// PRINT: Convert the evaluated result back to a string
char *PRINT(MalType *val)
{
    return pr_str(val, 1); // print_readably = 1
}

// Read-Eval-Print
char *rep(char *input, Env *env)
{
    MalType *ast = READ(input);
    if (!ast)
        return strdup("Error: Invalid input");

    MalType *result = EVAL(ast, env);
    if (!result)
    {
        mal_free(ast);
        return strdup("Error: Evaluation failed");
    }

    char *output = PRINT(result);

    // Free memory (note: in a real implementation, we'd need more sophisticated memory management)
    mal_free(ast);
    return output;
}

int main()
{
    // Create and initialize the environment with basic arithmetic functions
    Env *repl_env = env_new();
    env_set(repl_env, "+", add_func);
    env_set(repl_env, "-", sub_func);
    env_set(repl_env, "*", mul_func);
    env_set(repl_env, "/", div_func);

    // REPL loop
    char input[1024];
    printf("mal-user> ");
    while (fgets(input, sizeof(input), stdin))
    {
        // Remove trailing newline
        input[strcspn(input, "\n")] = 0;

        // Exit on empty input
        if (strlen(input) == 0)
        {
            printf("mal-user> ");
            continue;
        }

        // Process the input
        char *output = rep(input, repl_env);
        printf("%s\n", output);
        free(output);

        printf("mal-user> ");
    }

    // Clean up
    env_free(repl_env);
    return 0;
}
