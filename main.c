#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantic.h"

extern int yyparse(void);
extern FILE *yyin;
extern int yylineno;
extern int lex_error_count;

ASTNode *root_ast = NULL;
int syntax_error_count = 0;

int main(int argc, char **argv) {
    const char *filename = NULL;

    if (argc > 1) {
        filename = argv[1];
        yyin = fopen(filename, "r");
        if (!yyin) {
            fprintf(stderr, "Error: Could not open input file '%s'\n", filename);
            return 1;
        }
    } else {
        printf("Reading from standard input (Ctrl+D to finish)...\n");
        yyin = stdin;
    }

    printf("========================================\n");
    printf(" SWITCH-CASE MINI COMPILER FRONT END\n");
    printf("========================================\n");
    if (filename) {
        printf("Compiling source file: %s\n", filename);
    }
    printf("----------------------------------------\n");

    /* Phase 1 & 2: Lexical and Syntax Analysis */
    yylineno = 1;
    int parse_res = yyparse();

    if (yyin && yyin != stdin) {
        fclose(yyin);
    }

    if (lex_error_count > 0) {
        fprintf(stderr, "\n[FAILED] Lexical analysis completed with %d error(s).\n", lex_error_count);
        if (root_ast) free_ast(root_ast);
        printf("\nCompilation result: FAILURE (Lexical Errors)\n");
        return 1;
    }
    printf("Lexical analysis: completed successfully\n");

    if (parse_res != 0 || syntax_error_count > 0 || root_ast == NULL) {
        fprintf(stderr, "\n[FAILED] Syntax analysis completed with %d error(s).\n",
                syntax_error_count > 0 ? syntax_error_count : 1);
        if (root_ast) free_ast(root_ast);
        printf("\nCompilation result: FAILURE (Syntax Errors)\n");
        return 1;
    }
    printf("Syntax analysis: successful\n");

    /* Phase 3: AST Construction & Display */
    printf("AST construction: successful\n");
    printf("----------------------------------------\n");
    printf(" ABSTRACT SYNTAX TREE (AST)\n");
    printf("----------------------------------------\n");
    print_ast(root_ast, 0);
    printf("----------------------------------------\n");

    /* Phase 4: Semantic Analysis */
    int semantic_errors = perform_semantic_analysis(root_ast);
    if (semantic_errors > 0) {
        fprintf(stderr, "\n[FAILED] Semantic analysis completed with %d error(s).\n", semantic_errors);
        free_ast(root_ast);
        printf("\nCompilation result: FAILURE (Semantic Errors)\n");
        return 1;
    }

    printf("Semantic analysis: successful\n");
    printf("----------------------------------------\n");
    printf("Compilation result: SUCCESS\n");
    printf("========================================\n");

    free_ast(root_ast);
    return 0;
}
