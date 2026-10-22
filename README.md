# Switch-Case Mini Compiler Front End

A complete, working compiler front end for a C-like language centered around `switch`, `case`, `default`, and `break` statements, developed for the third-year B.Tech Compiler Design mini-project (Topic 12).

---

## 1. Problem Statement & Objectives

### Problem Statement
Develop a compiler front end supporting `switch`, `case`, `default`, and `break` statements. Validate case-label syntax and detect duplicate constant case labels.

### Key Objectives
1. **Lexical Analysis (Flex)**: Tokenize keywords, identifiers, integers, and operators while ignoring comments and whitespace, and tracking source lines.
2. **Syntax Analysis (GNU Bison)**: Parse program constructs according to a rigorous Context-Free Grammar with 0 shift/reduce conflicts.
3. **AST Construction**: Construct an Abstract Syntax Tree representing program structure, statements, switch selectors, and case clauses, with pretty-printing.
4. **Semantic Analysis**:
   - Detect and reject duplicate case labels within the same switch.
   - Allow identical case labels across separate switch statements.
   - Reject multiple `default` labels within a single switch.
   - Reject `break` statements outside a switch context (with proper nesting tracking).
   - Enforce symbol table rules (variable declaration before use, detect duplicate variable declarations).
   - Catch division by literal zero in constant expressions.
5. **Testing & Automation**: Comprehensive automated test suite with 28 tests covering valid programs, syntax errors, and semantic errors.

---

## 2. Technologies Used

- **Language**: C (C99/C11)
- **Lexical Analyzer Generator**: Flex (v2.6.4+)
- **Parser Generator**: GNU Bison (v3.8.2+)
- **Compiler**: GCC
- **Build System**: GNU Make
- **Environment**: WSL Ubuntu ARM64 / x86_64

---

## 3. Project Structure

```text
switchcasecompiler/
├── lexer.l                      # Flex scanner specification
├── parser.y                     # Bison grammar and parser specification
├── ast.h                        # AST node definitions and function prototypes
├── ast.c                        # AST construction, printing, and memory deallocation
├── semantic.h                   # Semantic analyzer and symbol table interfaces
├── semantic.c                   # Semantic validation passes (duplicate cases, symbol checks)
├── main.c                       # Compiler CLI entry point and pipeline coordinator
├── Makefile                     # Build system
├── run_tests.sh                 # Automated test runner script
├── input.txt                    # Demonstration source program
├── README.md                    # Project documentation
├── .gitignore                   # Git ignore file for build artifacts
├── docs/
│   ├── grammar.md               # CFG specification, associativity & transformations
│   ├── architecture.md          # Multi-phase compiler pipeline and module details
│   ├── setup.md                 # Setup, compilation, and troubleshooting guide
│   └── test-cases.md            # Test matrix and verification results
└── tests/
    ├── valid/                   # 11 Valid test programs
    ├── syntax_errors/           # 8 Syntax error test programs
    └── semantic_errors/         # 9 Semantic error test programs
```

---

## 4. Getting Started on WSL Ubuntu

### 4.1 Prerequisites Installation
```bash
sudo apt update
sudo apt install -y build-essential flex bison make git
```

### 4.2 Building the Compiler
```bash
make clean
make
```

### 4.3 Running the Compiler
With the demonstration input:
```bash
make run
```
Or directly:
```bash
./build/switch_compiler input.txt
```

### 4.4 Running the Automated Test Suite
```bash
make test
```

---

## 5. Example Walkthrough

### 5.1 Valid Input (`input.txt`)
```c
int choice = 2;
int result = 0;

switch (choice) {
    case 1:
        result = 10;
        print(result);
        break;

    case 2:
        result = 20;
        print(result);
        break;

    case 3:
        result = 30;
        print(result);
        break;

    default:
        result = 0;
        print(result);
        break;
}
```

#### Output:
```text
========================================
 SWITCH-CASE MINI COMPILER FRONT END
========================================
Compiling source file: input.txt
----------------------------------------
Lexical analysis: completed successfully
Syntax analysis: successful
AST construction: successful
----------------------------------------
 ABSTRACT SYNTAX TREE (AST)
----------------------------------------
Program (line 1)
  VarDecl: choice (line 1)
    InitExpr:
      IntegerLiteral: 2 (line 1)
  VarDecl: result (line 2)
    InitExpr:
      IntegerLiteral: 0 (line 2)
  SwitchStatement (line 24)
    Selector:
      Identifier: choice (line 4)
    Cases:
      CaseClause: 1 (line 10)
        Assign: result = (line 6)
          IntegerLiteral: 10 (line 6)
        Print (line 7)
          Identifier: result (line 7)
        BreakStatement (line 8)
      CaseClause: 2 (line 15)
        Assign: result = (line 11)
          IntegerLiteral: 20 (line 11)
        Print (line 12)
          Identifier: result (line 12)
        BreakStatement (line 13)
      CaseClause: 3 (line 20)
        Assign: result = (line 16)
          IntegerLiteral: 30 (line 16)
        Print (line 17)
          Identifier: result (line 17)
        BreakStatement (line 18)
      DefaultClause (line 24)
        Assign: result = (line 21)
          IntegerLiteral: 0 (line 21)
        Print (line 22)
          Identifier: result (line 22)
        BreakStatement (line 23)
----------------------------------------
Semantic analysis: successful
----------------------------------------
Compilation result: SUCCESS
========================================
```

### 5.2 Duplicate Case Error Demonstration
Input (`tests/semantic_errors/test01_duplicate_case.txt`):
```c
int choice = 1;
switch (choice) {
    case 1:
        print(10);
        break;
    case 1:
        print(20);
        break;
}
```

#### Error Output:
```text
Semantic Error at line 9: Duplicate case label: 1 (previously declared at line 6)

[FAILED] Semantic analysis completed with 1 error(s).

Compilation result: FAILURE (Semantic Errors)
```

---

## 6. Language Features and Limitations

### Supported Features
- Integer type declarations (`int x;`, `int x = 10;`).
- Variable assignments and arithmetic expressions (`+`, `-`, `*`, `/`, unary minus, parentheses).
- Switch statements with constant integer labels (`case 1:`, `case -5:`, `case +2:`).
- Independent label sets per switch block (nested switches supported).
- Optional `default` branch (max 1 per switch).
- `break;` statement validated inside switch contexts.
- `print(...)` built-in statement.
- Single-line (`//`) and multi-line (`/* */`) comments.

### Limitations
- Front-end only: Does not evaluate runtime code or produce target assembly/machine code.
- Data types: Supports 32-bit signed integer types (`int`); floating point and strings are excluded.
- Functions and loops (`for`, `while`) are intentionally excluded to maintain strict project scope.

---

## 7. Git & GitHub Setup

To commit and push the repository to GitHub from WSL:

```bash
# 1. Initialize Git repository (if not already done)
git init

# 2. Add files and make initial commit
git add .
git commit -m "feat: complete switch-case mini compiler front end"

# 3. Add remote and push (replace URL with your repository)
git branch -M main
git remote add origin https://github.com/<your-username>/switch-case-mini-compiler.git
git push -u origin main
```
