#!/usr/bin/env bash

TARGET="./build/switch_compiler"
TOTAL=0
PASSED=0
FAILED=0

echo "=========================================================="
echo " RUNNING SWITCH-CASE MINI COMPILER AUTOMATED TEST SUITE"
echo "=========================================================="

if [ ! -f "$TARGET" ]; then
    echo "Compiler binary not found at $TARGET. Building now..."
    make
fi

run_valid_test() {
    local file="$1"
    local name=$(basename "$file")
    TOTAL=$((TOTAL + 1))
    
    # Run compiler, expecting exit code 0
    $TARGET "$file" > /dev/null 2>&1
    local exit_code=$?
    
    if [ $exit_code -eq 0 ]; then
        echo "  [PASS] Valid Test: $name"
        PASSED=$((PASSED + 1))
    else
        echo "  [FAIL] Valid Test: $name (Expected exit code 0, got $exit_code)"
        FAILED=$((FAILED + 1))
    fi
}

run_error_test() {
    local file="$1"
    local category="$2"
    local name=$(basename "$file")
    TOTAL=$((TOTAL + 1))
    
    # Run compiler, expecting non-zero exit code
    $TARGET "$file" > /dev/null 2>&1
    local exit_code=$?
    
    if [ $exit_code -ne 0 ]; then
        echo "  [PASS] $category Error Test: $name (Correctly rejected with code $exit_code)"
        PASSED=$((PASSED + 1))
    else
        echo "  [FAIL] $category Error Test: $name (Expected failure, but exited with 0)"
        FAILED=$((FAILED + 1))
    fi
}

echo ""
echo "--- Category 1: Valid Programs (Expected: SUCCESS, Exit code 0) ---"
for f in tests/valid/*.txt; do
    [ -e "$f" ] || continue
    run_valid_test "$f"
done

echo ""
echo "--- Category 2: Syntax Error Programs (Expected: FAILURE, Exit code != 0) ---"
for f in tests/syntax_errors/*.txt; do
    [ -e "$f" ] || continue
    run_error_test "$f" "Syntax"
done

echo ""
echo "--- Category 3: Semantic Error Programs (Expected: FAILURE, Exit code != 0) ---"
for f in tests/semantic_errors/*.txt; do
    [ -e "$f" ] || continue
    run_error_test "$f" "Semantic"
done

echo ""
echo "=========================================================="
echo " TEST SUITE SUMMARY"
echo "=========================================================="
echo " Total Tests Run : $TOTAL"
echo " Passed          : $PASSED"
echo " Failed          : $FAILED"
echo "=========================================================="

if [ $FAILED -eq 0 ]; then
    echo "Result: ALL TESTS PASSED SUCCESSFULLY!"
    exit 0
else
    echo "Result: SOME TESTS FAILED!"
    exit 1
fi
