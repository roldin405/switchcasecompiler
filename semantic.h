#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

/* Symbol structure for symbol table */
typedef struct Symbol {
    char *name;
    int line;
    struct Symbol *next;
} Symbol;

/* Symbol table structure */
typedef struct SymbolTable {
    Symbol *head;
} SymbolTable;

/* Semantic analysis context */
typedef struct SemanticContext {
    SymbolTable *symtab;
    int switch_depth;
    int error_count;
} SemanticContext;

/* Main semantic analyzer entry point */
int perform_semantic_analysis(const ASTNode *root);

#endif /* SEMANTIC_H */
