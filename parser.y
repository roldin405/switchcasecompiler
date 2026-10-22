%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* Forward declarations */
int yylex(void);
void yyerror(const char *s);

extern int yylineno;
extern ASTNode *root_ast;
extern int syntax_error_count;
%}

%code requires {
    #include "ast.h"
}

%union {
    int int_val;
    char *str_val;
    ASTNode *node_val;
}

/* Token definitions */
%token TOKEN_SWITCH "switch"
%token TOKEN_CASE "case"
%token TOKEN_DEFAULT "default"
%token TOKEN_BREAK "break"
%token TOKEN_PRINT "print"
%token TOKEN_INT "int"

%token <str_val> TOKEN_IDENTIFIER
%token <int_val> TOKEN_INT_LITERAL

%token TOKEN_ASSIGN "="
%token TOKEN_SEMICOLON ";"
%token TOKEN_COLON ":"
%token TOKEN_LPAREN "("
%token TOKEN_RPAREN ")"
%token TOKEN_LBRACE "{"
%token TOKEN_RBRACE "}"
%token TOKEN_PLUS "+"
%token TOKEN_MINUS "-"
%token TOKEN_STAR "*"
%token TOKEN_SLASH "/"

/* Non-terminal types */
%type <node_val> program statement_list statement
%type <node_val> var_decl assignment print_stmt switch_stmt break_stmt
%type <node_val> case_list case_clause default_clause
%type <int_val> case_label_val
%type <node_val> expr term factor

%%

program:
    statement_list {
        root_ast = create_program_node($1, 1);
        $$ = root_ast;
    }
    | /* empty */ {
        root_ast = create_program_node(NULL, 1);
        $$ = root_ast;
    }
    ;

statement_list:
    statement {
        $$ = $1;
    }
    | statement_list statement {
        $$ = append_node($1, $2);
    }
    ;

statement:
    var_decl { $$ = $1; }
    | assignment { $$ = $1; }
    | print_stmt { $$ = $1; }
    | switch_stmt { $$ = $1; }
    | break_stmt { $$ = $1; }
    ;

var_decl:
    TOKEN_INT TOKEN_IDENTIFIER TOKEN_SEMICOLON {
        $$ = create_var_decl_node($2, NULL, yylineno);
        free($2);
    }
    | TOKEN_INT TOKEN_IDENTIFIER TOKEN_ASSIGN expr TOKEN_SEMICOLON {
        $$ = create_var_decl_node($2, $4, yylineno);
        free($2);
    }
    ;

assignment:
    TOKEN_IDENTIFIER TOKEN_ASSIGN expr TOKEN_SEMICOLON {
        $$ = create_assign_node($1, $3, yylineno);
        free($1);
    }
    ;

print_stmt:
    TOKEN_PRINT TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_SEMICOLON {
        $$ = create_print_node($3, yylineno);
    }
    ;

break_stmt:
    TOKEN_BREAK TOKEN_SEMICOLON {
        $$ = create_break_node(yylineno);
    }
    ;

switch_stmt:
    TOKEN_SWITCH TOKEN_LPAREN expr TOKEN_RPAREN TOKEN_LBRACE case_list TOKEN_RBRACE {
        $$ = create_switch_node($3, $6, yylineno);
    }
    ;

case_list:
    /* empty */ {
        $$ = NULL;
    }
    | case_list case_clause {
        $$ = append_node($1, $2);
    }
    | case_list default_clause {
        $$ = append_node($1, $2);
    }
    ;

case_label_val:
    TOKEN_INT_LITERAL {
        $$ = $1;
    }
    | TOKEN_PLUS TOKEN_INT_LITERAL {
        $$ = $2;
    }
    | TOKEN_MINUS TOKEN_INT_LITERAL {
        $$ = -$2;
    }
    ;

case_clause:
    TOKEN_CASE case_label_val TOKEN_COLON statement_list {
        $$ = create_case_node($2, $4, yylineno);
    }
    | TOKEN_CASE case_label_val TOKEN_COLON {
        $$ = create_case_node($2, NULL, yylineno);
    }
    ;

default_clause:
    TOKEN_DEFAULT TOKEN_COLON statement_list {
        $$ = create_default_node($3, yylineno);
    }
    | TOKEN_DEFAULT TOKEN_COLON {
        $$ = create_default_node(NULL, yylineno);
    }
    ;

expr:
    expr TOKEN_PLUS term {
        $$ = create_bin_expr_node(OP_ADD, $1, $3, yylineno);
    }
    | expr TOKEN_MINUS term {
        $$ = create_bin_expr_node(OP_SUB, $1, $3, yylineno);
    }
    | term {
        $$ = $1;
    }
    ;

term:
    term TOKEN_STAR factor {
        $$ = create_bin_expr_node(OP_MUL, $1, $3, yylineno);
    }
    | term TOKEN_SLASH factor {
        $$ = create_bin_expr_node(OP_DIV, $1, $3, yylineno);
    }
    | factor {
        $$ = $1;
    }
    ;

factor:
    TOKEN_INT_LITERAL {
        $$ = create_int_literal_node($1, yylineno);
    }
    | TOKEN_IDENTIFIER {
        $$ = create_identifier_node($1, yylineno);
        free($1);
    }
    | TOKEN_LPAREN expr TOKEN_RPAREN {
        $$ = $2;
    }
    | TOKEN_PLUS factor {
        $$ = $2;
    }
    | TOKEN_MINUS factor {
        $$ = create_unary_expr_node(OP_SUB, $2, yylineno);
    }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error at line %d: %s\n", yylineno, s);
    syntax_error_count++;
}
