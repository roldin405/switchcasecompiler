#include "semantic.h"
#include <string.h>

/* Forward declarations */
static void check_node(const ASTNode *node, SemanticContext *ctx);
static void check_expr(const ASTNode *node, SemanticContext *ctx);

/* Symbol table functions */
static SymbolTable *create_symbol_table(void) {
    SymbolTable *st = (SymbolTable *)calloc(1, sizeof(SymbolTable));
    return st;
}

static int add_symbol(SymbolTable *st, const char *name, int line) {
    if (!st || !name) return 0;

    /* Check for duplicate declaration in current table */
    for (Symbol *curr = st->head; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) {
            return 0; /* Duplicate found */
        }
    }

    Symbol *sym = (Symbol *)calloc(1, sizeof(Symbol));
    sym->name = strdup(name);
    sym->line = line;
    sym->next = st->head;
    st->head = sym;
    return 1; /* Successfully added */
}

static int lookup_symbol(const SymbolTable *st, const char *name) {
    if (!st || !name) return 0;
    for (const Symbol *curr = st->head; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) {
            return 1; /* Found */
        }
    }
    return 0; /* Not found */
}

static void free_symbol_table(SymbolTable *st) {
    if (!st) return;
    Symbol *curr = st->head;
    while (curr) {
        Symbol *temp = curr->next;
        free(curr->name);
        free(curr);
        curr = temp;
    }
    free(st);
}

/* Helper for per-switch case value collection */
typedef struct CaseValueEntry {
    int val;
    int line;
    struct CaseValueEntry *next;
} CaseValueEntry;

static void check_switch(const ASTNode *node, SemanticContext *ctx) {
    /* 1. Check switch selector expression */
    if (node->left) {
        check_expr(node->left, ctx);
    }

    /* 2. Validate cases and defaults */
    CaseValueEntry *case_values = NULL;
    int default_count = 0;
    int default_first_line = 0;

    /* Increase switch depth for nested break checking */
    ctx->switch_depth++;

    for (const ASTNode *clause = node->body; clause; clause = clause->next) {
        if (clause->type == NODE_CASE) {
            /* Check duplicate constant case label */
            int is_dup = 0;
            int first_seen_line = 0;
            for (CaseValueEntry *entry = case_values; entry; entry = entry->next) {
                if (entry->val == clause->int_val) {
                    is_dup = 1;
                    first_seen_line = entry->line;
                    break;
                }
            }

            if (is_dup) {
                fprintf(stderr, "Semantic Error at line %d: Duplicate case label: %d (previously declared at line %d)\n",
                        clause->line, clause->int_val, first_seen_line);
                ctx->error_count++;
            } else {
                CaseValueEntry *new_entry = (CaseValueEntry *)malloc(sizeof(CaseValueEntry));
                new_entry->val = clause->int_val;
                new_entry->line = clause->line;
                new_entry->next = case_values;
                case_values = new_entry;
            }

            /* Check statements inside case body */
            for (const ASTNode *s = clause->body; s; s = s->next) {
                check_node(s, ctx);
            }
        } else if (clause->type == NODE_DEFAULT) {
            default_count++;
            if (default_count == 1) {
                default_first_line = clause->line;
            } else {
                fprintf(stderr, "Semantic Error at line %d: Multiple default labels in switch (first default at line %d)\n",
                        clause->line, default_first_line);
                ctx->error_count++;
            }

            /* Check statements inside default body */
            for (const ASTNode *s = clause->body; s; s = s->next) {
                check_node(s, ctx);
            }
        }
    }

    /* Free per-switch case value list */
    CaseValueEntry *entry = case_values;
    while (entry) {
        CaseValueEntry *tmp = entry->next;
        free(entry);
        entry = tmp;
    }

    ctx->switch_depth--;
}

static void check_expr(const ASTNode *node, SemanticContext *ctx) {
    if (!node) return;

    switch (node->type) {
        case NODE_INT_LITERAL:
            /* Int literals are always valid */
            break;

        case NODE_IDENTIFIER:
            if (!lookup_symbol(ctx->symtab, node->name)) {
                fprintf(stderr, "Semantic Error at line %d: Use of undeclared identifier '%s'\n",
                        node->line, node->name);
                ctx->error_count++;
            }
            break;

        case NODE_BIN_EXPR:
            check_expr(node->left, ctx);
            check_expr(node->right, ctx);

            /* Check literal division by zero */
            if (node->op == OP_DIV && node->right && node->right->type == NODE_INT_LITERAL && node->right->int_val == 0) {
                fprintf(stderr, "Semantic Error at line %d: Division by zero detected in constant expression\n",
                        node->line);
                ctx->error_count++;
            }
            break;

        case NODE_UNARY_EXPR:
            check_expr(node->left, ctx);
            break;

        default:
            break;
    }
}

static void check_node(const ASTNode *node, SemanticContext *ctx) {
    if (!node) return;

    switch (node->type) {
        case NODE_PROGRAM:
            for (const ASTNode *s = node->body; s; s = s->next) {
                check_node(s, ctx);
            }
            break;

        case NODE_VAR_DECL:
            /* Check initializer if present */
            if (node->left) {
                check_expr(node->left, ctx);
            }
            /* Add to symbol table */
            if (!add_symbol(ctx->symtab, node->name, node->line)) {
                fprintf(stderr, "Semantic Error at line %d: Redeclaration of variable '%s'\n",
                        node->line, node->name);
                ctx->error_count++;
            }
            break;

        case NODE_ASSIGN:
            /* Check if target variable is declared */
            if (!lookup_symbol(ctx->symtab, node->name)) {
                fprintf(stderr, "Semantic Error at line %d: Assignment to undeclared variable '%s'\n",
                        node->line, node->name);
                ctx->error_count++;
            }
            /* Check right-hand expression */
            if (node->left) {
                check_expr(node->left, ctx);
            }
            break;

        case NODE_PRINT:
            if (node->left) {
                check_expr(node->left, ctx);
            }
            break;

        case NODE_SWITCH:
            check_switch(node, ctx);
            break;

        case NODE_BREAK:
            if (ctx->switch_depth <= 0) {
                fprintf(stderr, "Semantic Error at line %d: 'break' statement outside of switch\n",
                        node->line);
                ctx->error_count++;
            }
            break;

        default:
            /* Other expressions or statements */
            break;
    }
}

int perform_semantic_analysis(const ASTNode *root) {
    if (!root) return 0;

    SemanticContext ctx;
    ctx.symtab = create_symbol_table();
    ctx.switch_depth = 0;
    ctx.error_count = 0;

    check_node(root, &ctx);

    free_symbol_table(ctx.symtab);

    return ctx.error_count;
}
