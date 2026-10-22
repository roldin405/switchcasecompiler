# Test Cases Specification & Verification Matrix

The test suite contains **28 comprehensive test cases** covering valid programs, syntax errors, and semantic errors.

All tests are verified automatically via `make test` (`run_tests.sh`).

---

## 1. Valid Programs (Expected Exit Code: 0)

| Test File | Description | Verification Status |
|:---|:---|:---:|
| `tests/valid/test01_single_case.txt` | Switch with single case and break | PASSED |
| `tests/valid/test02_multiple_cases.txt` | Switch with multiple distinct constant cases | PASSED |
| `tests/valid/test03_with_default.txt` | Switch with case and a default branch | PASSED |
| `tests/valid/test04_no_default.txt` | Switch without default branch | PASSED |
| `tests/valid/test05_initialized_var.txt` | Switch on initialized integer variable | PASSED |
| `tests/valid/test06_expr_selector.txt` | Switch on arithmetic expression (`a + b * 3`) | PASSED |
| `tests/valid/test07_assign_print.txt` | Variable assignments and print statements in case | PASSED |
| `tests/valid/test08_multiple_switches.txt`| Separate switch statements using identical case labels | PASSED |
| `tests/valid/test09_negative_case.txt` | Negative integer constant case labels (`case -10:`) | PASSED |
| `tests/valid/test10_multiple_stmts_in_case.txt` | Case block containing multiple statements | PASSED |
| `tests/valid/test11_nested_switch.txt` | Nested switch statement blocks | PASSED |

---

## 2. Syntax Error Programs (Expected Exit Code: != 0)

| Test File | Description | Error Caught | Verification Status |
|:---|:---|:---|:---:|
| `tests/syntax_errors/test01_missing_open_brace.txt` | Missing opening brace after switch selector | Syntax Error | PASSED |
| `tests/syntax_errors/test02_missing_close_brace.txt`| Missing closing brace at end of switch | Syntax Error | PASSED |
| `tests/syntax_errors/test03_missing_colon.txt` | Missing colon `:` after case constant | Syntax Error | PASSED |
| `tests/syntax_errors/test04_missing_semicolon.txt` | Missing semicolon `;` after assignment statement | Syntax Error | PASSED |
| `tests/syntax_errors/test05_missing_parens.txt` | Missing parentheses around switch selector | Syntax Error | PASSED |
| `tests/syntax_errors/test06_malformed_case_label.txt`| Empty case constant (`case:`) | Syntax Error | PASSED |
| `tests/syntax_errors/test07_invalid_char.txt` | Unrecognized symbol `@` in source | Lexical / Syntax Error | PASSED |
| `tests/syntax_errors/test08_malformed_expr.txt` | Invalid operator sequence (`5 + * 2`) | Syntax Error | PASSED |

---

## 3. Semantic Error Programs (Expected Exit Code: != 0)

| Test File | Description | Error Diagnostic | Verification Status |
|:---|:---|:---|:---:|
| `tests/semantic_errors/test01_duplicate_case.txt` | Duplicate case constant `1` in same switch | `Duplicate case label: 1` | PASSED |
| `tests/semantic_errors/test02_multiple_defaults.txt`| Two `default:` labels in single switch | `Multiple default labels in switch` | PASSED |
| `tests/semantic_errors/test03_undeclared_selector.txt`| Switch on undeclared identifier | `Use of undeclared identifier` | PASSED |
| `tests/semantic_errors/test04_undeclared_in_expr.txt`| Undeclared variable used in print expression | `Use of undeclared identifier` | PASSED |
| `tests/semantic_errors/test05_undeclared_assign.txt`| Assignment to undeclared identifier | `Assignment to undeclared variable` | PASSED |
| `tests/semantic_errors/test06_duplicate_var_decl.txt`| Redeclaration of same variable name | `Redeclaration of variable` | PASSED |
| `tests/semantic_errors/test07_break_outside_switch.txt`| `break;` statement outside any switch block | `'break' statement outside of switch` | PASSED |
| `tests/semantic_errors/test08_equivalent_labels.txt`| Equivalent labels `case 5:` and `case +5:` | `Duplicate case label: 5` | PASSED |
| `tests/semantic_errors/test09_div_by_zero.txt` | Division by literal `0` in constant expression | `Division by zero detected` | PASSED |

---

## 4. Verification Summary

- **Total Tests**: 28
- **Passed**: 28
- **Failed**: 0
- **Pass Rate**: 100%
