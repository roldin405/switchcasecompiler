%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"

int lex_error_count = 0;
%}

%option noyywrap
%option yylineno
%option nounput
%option noinput

DIGIT       [0-9]
LETTER      [a-zA-Z_]
ID          {LETTER}{LETTER}*
INT_CONST   {DIGIT}+

%%

[ \t\r]+                { /* Ignore whitespace */ }
\n                      { /* Handled by yylineno automatically */ }

"//".*                  { /* Single-line comment */ }
"/*"([^*]|\*+[^*/])*\*+"/" { /* Multi-line comment */ }

"switch"                { return TOKEN_SWITCH; }
"case"                  { return TOKEN_CASE; }
"default"               { return TOKEN_DEFAULT; }
"break"                 { return TOKEN_BREAK; }
"print"                 { return TOKEN_PRINT; }
"int"                   { return TOKEN_INT; }

"="                     { return TOKEN_ASSIGN; }
";"                     { return TOKEN_SEMICOLON; }
":"                     { return TOKEN_COLON; }
"("                     { return TOKEN_LPAREN; }
")"                     { return TOKEN_RPAREN; }
"{"                     { return TOKEN_LBRACE; }
"}"                     { return TOKEN_RBRACE; }
"+"                     { return TOKEN_PLUS; }
"-"                     { return TOKEN_MINUS; }
"*"                     { return TOKEN_STAR; }
"/"                     { return TOKEN_SLASH; }

{INT_CONST}             {
                            yylval.int_val = atoi(yytext);
                            return TOKEN_INT_LITERAL;
                        }

{LETTER}({LETTER}|{DIGIT})* {
                            yylval.str_val = strdup(yytext);
                            return TOKEN_IDENTIFIER;
                        }

.                       {
                            fprintf(stderr, "Lexical Error at line %d: Unrecognized character '%s'\n", yylineno, yytext);
                            lex_error_count++;
                        }

%%
