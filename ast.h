#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    NODE_PROGRAM,
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_PRINT,
    NODE_SWITCH,
    NODE_CASE,
    NODE_DEFAULT,
    NODE_BREAK,
    NODE_BIN_EXPR,
    NODE_UNARY_EXPR,
    NODE_INT_LITERAL,
    NODE_IDENTIFIER
} NodeType;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
} BinaryOpType;

typedef struct ASTNode {
    NodeType type;
    int line;

    /* Value attributes */
    char *name;         /* Variable name or identifier */
    int int_val;        /* Integer constant */
    BinaryOpType op;    /* Binary operator (+, -, *, /) */

    /* Subtrees / Children */
    struct ASTNode *left;   /* E.g. expr in assign/binop, selector in switch, init expr in decl */
    struct ASTNode *right;  /* E.g. right operand in binop */
    struct ASTNode *body;   /* Statements inside case/default or program */

    /* Linked list next pointer for statements / case lists */
    struct ASTNode *next;
} ASTNode;

/* Factory functions */
ASTNode *create_program_node(ASTNode *stmts, int line);
ASTNode *create_var_decl_node(const char *name, ASTNode *init_expr, int line);
ASTNode *create_assign_node(const char *name, ASTNode *expr, int line);
ASTNode *create_print_node(ASTNode *expr, int line);
ASTNode *create_switch_node(ASTNode *expr, ASTNode *cases, int line);
ASTNode *create_case_node(int const_val, ASTNode *body, int line);
ASTNode *create_default_node(ASTNode *body, int line);
ASTNode *create_break_node(int line);
ASTNode *create_bin_expr_node(BinaryOpType op, ASTNode *left, ASTNode *right, int line);
ASTNode *create_unary_expr_node(BinaryOpType op, ASTNode *child, int line);
ASTNode *create_int_literal_node(int val, int line);
ASTNode *create_identifier_node(const char *name, int line);

/* Helper to append to linked-list of nodes */
ASTNode *append_node(ASTNode *head, ASTNode *new_node);

/* Pretty print AST */
void print_ast(const ASTNode *node, int indent);

/* Free AST memory */
void free_ast(ASTNode *node);

#endif /* AST_H */
