#include "ast.h"
#include <string.h>

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    char *copy = (char *)malloc(strlen(s) + 1);
    if (copy) {
        strcpy(copy, s);
    }
    return copy;
}

static ASTNode *allocate_node(NodeType type, int line) {
    ASTNode *node = (ASTNode *)calloc(1, sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Fatal Error: Out of memory during AST node allocation at line %d\n", line);
        exit(1);
    }
    node->type = type;
    node->line = line;
    return node;
}

ASTNode *create_program_node(ASTNode *stmts, int line) {
    ASTNode *node = allocate_node(NODE_PROGRAM, line);
    node->body = stmts;
    return node;
}

ASTNode *create_var_decl_node(const char *name, ASTNode *init_expr, int line) {
    ASTNode *node = allocate_node(NODE_VAR_DECL, line);
    node->name = safe_strdup(name);
    node->left = init_expr;
    return node;
}

ASTNode *create_assign_node(const char *name, ASTNode *expr, int line) {
    ASTNode *node = allocate_node(NODE_ASSIGN, line);
    node->name = safe_strdup(name);
    node->left = expr;
    return node;
}

ASTNode *create_print_node(ASTNode *expr, int line) {
    ASTNode *node = allocate_node(NODE_PRINT, line);
    node->left = expr;
    return node;
}

ASTNode *create_switch_node(ASTNode *expr, ASTNode *cases, int line) {
    ASTNode *node = allocate_node(NODE_SWITCH, line);
    node->left = expr;   /* selector expression */
    node->body = cases;  /* list of case and default clauses */
    return node;
}

ASTNode *create_case_node(int const_val, ASTNode *body, int line) {
    ASTNode *node = allocate_node(NODE_CASE, line);
    node->int_val = const_val;
    node->body = body;   /* statements inside this case */
    return node;
}

ASTNode *create_default_node(ASTNode *body, int line) {
    ASTNode *node = allocate_node(NODE_DEFAULT, line);
    node->body = body;   /* statements inside default */
    return node;
}

ASTNode *create_break_node(int line) {
    return allocate_node(NODE_BREAK, line);
}

ASTNode *create_bin_expr_node(BinaryOpType op, ASTNode *left, ASTNode *right, int line) {
    ASTNode *node = allocate_node(NODE_BIN_EXPR, line);
    node->op = op;
    node->left = left;
    node->right = right;
    return node;
}

ASTNode *create_unary_expr_node(BinaryOpType op, ASTNode *child, int line) {
    ASTNode *node = allocate_node(NODE_UNARY_EXPR, line);
    node->op = op;
    node->left = child;
    return node;
}

ASTNode *create_int_literal_node(int val, int line) {
    ASTNode *node = allocate_node(NODE_INT_LITERAL, line);
    node->int_val = val;
    return node;
}

ASTNode *create_identifier_node(const char *name, int line) {
    ASTNode *node = allocate_node(NODE_IDENTIFIER, line);
    node->name = safe_strdup(name);
    return node;
}

ASTNode *append_node(ASTNode *head, ASTNode *new_node) {
    if (!head) return new_node;
    if (!new_node) return head;

    ASTNode *curr = head;
    while (curr->next) {
        curr = curr->next;
    }
    curr->next = new_node;
    return head;
}

static void print_indent(int indent) {
    for (int i = 0; i < indent; ++i) {
        printf("  ");
    }
}

static const char *op_to_string(BinaryOpType op) {
    switch (op) {
        case OP_ADD: return "+";
        case OP_SUB: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
        default: return "?";
    }
}

void print_ast(const ASTNode *node, int indent) {
    if (!node) return;

    print_indent(indent);

    switch (node->type) {
        case NODE_PROGRAM:
            printf("Program (line %d)\n", node->line);
            for (ASTNode *curr = node->body; curr; curr = curr->next) {
                print_ast(curr, indent + 1);
            }
            break;

        case NODE_VAR_DECL:
            printf("VarDecl: %s (line %d)\n", node->name ? node->name : "<anon>", node->line);
            if (node->left) {
                print_indent(indent + 1);
                printf("InitExpr:\n");
                print_ast(node->left, indent + 2);
            }
            break;

        case NODE_ASSIGN:
            printf("Assign: %s = (line %d)\n", node->name ? node->name : "<anon>", node->line);
            print_ast(node->left, indent + 1);
            break;

        case NODE_PRINT:
            printf("Print (line %d)\n", node->line);
            print_ast(node->left, indent + 1);
            break;

        case NODE_SWITCH:
            printf("SwitchStatement (line %d)\n", node->line);
            print_indent(indent + 1);
            printf("Selector:\n");
            print_ast(node->left, indent + 2);
            print_indent(indent + 1);
            printf("Cases:\n");
            for (ASTNode *curr = node->body; curr; curr = curr->next) {
                print_ast(curr, indent + 2);
            }
            break;

        case NODE_CASE:
            printf("CaseClause: %d (line %d)\n", node->int_val, node->line);
            if (node->body) {
                for (ASTNode *curr = node->body; curr; curr = curr->next) {
                    print_ast(curr, indent + 1);
                }
            } else {
                print_indent(indent + 1);
                printf("(empty body)\n");
            }
            break;

        case NODE_DEFAULT:
            printf("DefaultClause (line %d)\n", node->line);
            if (node->body) {
                for (ASTNode *curr = node->body; curr; curr = curr->next) {
                    print_ast(curr, indent + 1);
                }
            } else {
                print_indent(indent + 1);
                printf("(empty body)\n");
            }
            break;

        case NODE_BREAK:
            printf("BreakStatement (line %d)\n", node->line);
            break;

        case NODE_BIN_EXPR:
            printf("BinaryExpr (%s) (line %d)\n", op_to_string(node->op), node->line);
            print_ast(node->left, indent + 1);
            print_ast(node->right, indent + 1);
            break;

        case NODE_UNARY_EXPR:
            printf("UnaryExpr (%s) (line %d)\n", op_to_string(node->op), node->line);
            print_ast(node->left, indent + 1);
            break;

        case NODE_INT_LITERAL:
            printf("IntegerLiteral: %d (line %d)\n", node->int_val, node->line);
            break;

        case NODE_IDENTIFIER:
            printf("Identifier: %s (line %d)\n", node->name ? node->name : "<null>", node->line);
            break;

        default:
            printf("UnknownNode (%d)\n", node->type);
            break;
    }
}

void free_ast(ASTNode *node) {
    if (!node) return;

    if (node->name) {
        free(node->name);
        node->name = NULL;
    }

    if (node->left) {
        free_ast(node->left);
        node->left = NULL;
    }

    if (node->right) {
        free_ast(node->right);
        node->right = NULL;
    }

    if (node->body) {
        free_ast(node->body);
        node->body = NULL;
    }

    if (node->next) {
        free_ast(node->next);
        node->next = NULL;
    }

    free(node);
}
