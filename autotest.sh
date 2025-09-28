#!/bin/bash
# autotest.sh - automated tests for gwatch

set -e

BUILD_DIR=build
DEBUGGER=$BUILD_DIR/gwatch
TEST_PROG=$BUILD_DIR/tests/real  # default to tests/real

# parse options
while getopts "b:d:t:" opt; do
  case $opt in
    b) BUILD_DIR=$OPTARG ;;
    d) DEBUGGER=$OPTARG ;;
    t) TEST_PROG=$OPTARG ;;
    *) echo "Usage: $0 [-b build_dir] [-d debugger] [-t test_prog]"; exit 1 ;;
  esac
done

# 1. Build everything
cmake -S . -B $BUILD_DIR
cmake --build $BUILD_DIR

# 2. Define test cases: args = number of accesses
declare -a TEST_CASES=(100 1000 5000)

PASS_COUNT=0
TOTAL_COUNT=${#TEST_CASES[@]}

for N in "${TEST_CASES[@]}"; do
    echo "Running test with $N accesses..."

    # Run debugger on sample program
    $DEBUGGER --var global_var --exec $TEST_PROG $N > output.txt

    # Basic verification: ensure some read/write events were captured
    READ_COUNT=$(grep -c "read:" output.txt || true)
    WRITE_COUNT=$(grep -c "write:" output.txt || true)

    EXPECTED_COUNT=$N
    ACTUAL_COUNT=$((READ_COUNT + WRITE_COUNT))

    if [ "$ACTUAL_COUNT" -eq "$EXPECTED_COUNT" ]; then
        echo "Test $N passed ($ACTUAL_COUNT events)"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        echo "Test $N FAILED: expected $EXPECTED_COUNT events, got $ACTUAL_COUNT"
    fi
done

echo "Autotest complete: $PASS_COUNT/$TOTAL_COUNT tests passed"
if [ $PASS_COUNT -ne $TOTAL_COUNT ]; then
    exit 1
fi