# Context-Free Grammar (CFG) Specification

## 1. Introduction

This document formally describes the Context-Free Grammar (CFG) for the **Switch-Case Mini Compiler Front End**. The grammar defines variable declarations, assignments, print statements, arithmetic expressions, and `switch-case-default-break` constructs.

The grammar is implemented in GNU Bison using an LALR(1) parser.

---

## 2. Terminals and Non-Terminals

### 2.1 Terminals (Tokens)
- **Keywords**: `TOKEN_SWITCH` (`switch`), `TOKEN_CASE` (`case`), `TOKEN_DEFAULT` (`default`), `TOKEN_BREAK` (`break`), `TOKEN_PRINT` (`print`), `TOKEN_INT` (`int`)
- **Identifiers & Literals**: `TOKEN_IDENTIFIER`, `TOKEN_INT_LITERAL`
- **Punctuation & Delimiters**: `;` (`TOKEN_SEMICOLON`), `:` (`TOKEN_COLON`), `(` (`TOKEN_LPAREN`), `)` (`TOKEN_RPAREN`), `{` (`TOKEN_LBRACE`), `}` (`TOKEN_RBRACE`), `=` (`TOKEN_ASSIGN`)
- **Arithmetic Operators**: `+` (`TOKEN_PLUS`), `-` (`TOKEN_MINUS`), `*` (`TOKEN_STAR`), `/` (`TOKEN_SLASH`)

### 2.2 Non-Terminals
- `program`: The top-level root symbol
- `statement_list`: Sequence of one or more statements
- `statement`: An individual statement (declaration, assignment, print, switch, break)
- `var_decl`: Variable declaration with optional initialization
- `assignment`: Variable assignment
- `print_stmt`: Built-in print statement
- `switch_stmt`: Switch block construct
- `break_stmt`: Break statement
- `case_list`: Sequence of case clauses and default clause
- `case_clause`: An individual case clause (`case <const>: <stmts>`)
- `default_clause`: Default fallback branch (`default: <stmts>`)
- `case_label_val`: Signed integer constant label
- `expr`: Addition and subtraction expressions
- `term`: Multiplication and division expressions
- `factor`: Primary factors (literals, identifiers, parentheses, unary operators)

---

## 3. Production Rules

Using standard BNF notation with $\varepsilon$ indicating an empty production:

```bnf
program ::= statement_list
          | ε

statement_list ::= statement
                 | statement_list statement

statement ::= var_decl
            | assignment
            | print_stmt
            | switch_stmt
            | break_stmt

var_decl ::= "int" IDENTIFIER ";"
           | "int" IDENTIFIER "=" expr ";"

assignment ::= IDENTIFIER "=" expr ";"

print_stmt ::= "print" "(" expr ")" ";"

break_stmt ::= "break" ";"

switch_stmt ::= "switch" "(" expr ")" "{" case_list "}"

case_list ::= ε
            | case_list case_clause
            | case_list default_clause

case_label_val ::= INT_LITERAL
                 | "+" INT_LITERAL
                 | "-" INT_LITERAL

case_clause ::= "case" case_label_val ":" statement_list
              | "case" case_label_val ":"

default_clause ::= "default" ":" statement_list
                 | "default" ":"

expr ::= expr "+" term
       | expr "-" term
       | term

term ::= term "*" factor
       | term "/" factor
       | factor

factor ::= INT_LITERAL
         | IDENTIFIER
         | "(" expr ")"
         | "+" factor
         | "-" factor
```

---

## 4. Grammar Design & Transformations

### 4.1 Operator Precedence and Associativity
Arithmetic expressions are decomposed into three levels (`expr`, `term`, `factor`):
- **Precedence Hierarchy**:
  1. Primary and Unary (`(expr)`, identifier, literal, unary `+`, `-`) — Highest
  2. Multiplicative (`*`, `/`) — Middle
  3. Additive (`+`, `-`) — Lowest
- **Associativity**:
  - `expr -> expr + term` and `term -> term * factor` use **left recursion**, which enforces standard left-to-right associativity (e.g., `10 - 5 - 2` parses as `(10 - 5) - 2`).
  - LALR(1) parsers like Bison naturally and efficiently parse left-recursive rules using $O(1)$ stack space per reduced rule, avoiding stack overflow typical of top-down parsers.

### 4.2 Ambiguity and Shift/Reduce Conflicts
- The grammar was designed with explicit statement terminators (`;`) and clear structural delimiters (`{`, `}`, `(`, `)`).
- The `case_list` and statement rules produce **0 shift/reduce conflicts** and **0 reduce/reduce conflicts** in Bison.
