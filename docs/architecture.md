# Compiler Architecture & Pipeline

## 1. Overview

The **Switch-Case Mini Compiler Front End** follows the classical multi-phase compiler pipeline:

```
Source Code (.txt / .c)
          │
          ▼
┌──────────────────┐
│ Lexical Analysis │  lexer.l (Flex)
└──────────────────┘
          │ Tokens (with yylineno)
          ▼
┌──────────────────┐
│ Syntax Analysis  │  parser.y (GNU Bison)
└──────────────────┘
          │ Syntax Tree Construction
          ▼
┌──────────────────┐
│ AST Construction │  ast.c, ast.h
└──────────────────┘
          │ Abstract Syntax Tree (Root node)
          ▼
┌──────────────────┐
│Semantic Analysis │  semantic.c, semantic.h
└──────────────────┘
          │
          ▼
 Compilation Result (Terminal Summary & Status Code)
```

---

## 2. Component Responsibilities

### 2.1 Lexical Analysis (`lexer.l`)
- Built using **Flex**.
- Scans raw character stream and extracts language tokens (`switch`, `case`, `default`, `break`, `print`, `int`, identifiers, integers, symbols).
- Strips whitespace, single-line comments (`//`), and block comments (`/* ... */`).
- Tracks line numbers automatically using `%option yylineno`.
- Reports lexical errors with exact line numbers and offending characters.

### 2.2 Syntax Analysis (`parser.y`)
- Built using **GNU Bison** (LALR(1) parser).
- Implements the Context-Free Grammar.
- Verifies grammatical structure: declarations, assignments, switch blocks, case lists, expressions.
- Directs semantic actions to build AST nodes rather than evaluating expressions immediately.
- Reports syntax errors via `yyerror` with line numbers.

### 2.3 Abstract Syntax Tree (AST) (`ast.h`, `ast.c`)
- Represents program structure hierarchically.
- Node types:
  - `NODE_PROGRAM`: Program root holding the statement list.
  - `NODE_VAR_DECL`: Variable declaration with optional initialization expression.
  - `NODE_ASSIGN`: Assignment statement (`variable = expr`).
  - `NODE_PRINT`: Built-in print statement.
  - `NODE_SWITCH`: Switch statement holding selector expression and case list.
  - `NODE_CASE`: Case clause holding integer value and body statements.
  - `NODE_DEFAULT`: Default clause holding body statements.
  - `NODE_BREAK`: Break statement.
  - `NODE_BIN_EXPR`: Binary arithmetic operations (`+`, `-`, `*`, `/`).
  - `NODE_UNARY_EXPR`: Unary operations (e.g. `-factor`).
  - `NODE_INT_LITERAL`: Integer numeric constant.
  - `NODE_IDENTIFIER`: Variable identifier.
- Provides `print_ast()` for human-readable tree visualization.
- Provides recursive memory cleanup (`free_ast()`).

### 2.4 Semantic Analysis (`semantic.h`, `semantic.c`)
- Inspects the constructed AST independently of parsing.
- Key checks implemented:
  1. **Duplicate Case-Label Detection**: Uses a dynamic per-switch collection of integer values. Duplicate labels within the same switch are rejected, while identical labels in different switches are accepted.
  2. **Default-Label Validation**: Ensures at most one `default` label per switch.
  3. **Case-Label Syntax Validation**: Enforces constant integers (positive and negative) for case labels at the grammar/AST level.
  4. **Symbol Table & Scope Management**: Tracks declared variables; flags undeclared identifiers in expressions, assignments, and switch selectors; rejects redeclarations.
  5. **Break Validation**: Verifies that `break` statements only occur inside `switch` constructs (`switch_depth > 0`). Handles nested switches properly.
  6. **Division-by-Zero Detection**: Identifies division by literal `0` in constant expressions.

### 2.5 Driver & CLI (`main.c`)
- Coordinates the execution pipeline.
- Handles command line arguments for file input and standard input.
- Displays the AST upon parsing success.
- Prints status summary and returns exit code `0` on success, `1` on error.
