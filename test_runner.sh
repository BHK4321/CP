#!/bin/bash
# Bash Test Runner Script (for Linux/macOS)
# Usage: ./test_runner.sh [solution_file] [test_cases_dir]
# Example: ./test_runner.sh solution.cpp test_cases

# Default parameters
SOLUTION_FILE=${1:-"solution.cpp"}
TEST_CASES_DIR=${2:-"test_cases"}
COMPILER=${3:-"g++"}
REQUIREMENTS_FILE="requirements.json"

# Default limits
TIME_LIMIT=4
MEMORY_LIMIT=512

# Load requirements if file exists
if [ -f "$REQUIREMENTS_FILE" ]; then
    if command -v jq >/dev/null 2>&1; then
        TIME_LIMIT=$(jq -r '.time // 4' "$REQUIREMENTS_FILE")
        MEMORY_LIMIT=$(jq -r '.space // 512' "$REQUIREMENTS_FILE")
        echo -e "${BLUE}Loaded requirements: ${TIME_LIMIT}s time, ${MEMORY_LIMIT}MB memory${NC}"
    else
        echo -e "${YELLOW}Warning: jq not found, using default limits${NC}"
    fi
fi

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== C++ Solution Test Runner ===${NC}"
echo -e "${BLUE}Solution: $SOLUTION_FILE${NC}"
echo -e "${BLUE}Test Cases Directory: $TEST_CASES_DIR${NC}"
echo -e "${BLUE}Time Limit: ${TIME_LIMIT}s, Memory Limit: ${MEMORY_LIMIT}MB${NC}"
echo ""

# Check if solution file exists
if [ ! -f "$SOLUTION_FILE" ]; then
    echo -e "${RED}Error: Solution file '$SOLUTION_FILE' not found!${NC}"
    exit 1
fi

# Check if test cases directory exists
if [ ! -d "$TEST_CASES_DIR" ]; then
    echo -e "${RED}Error: Test cases directory '$TEST_CASES_DIR' not found!${NC}"
    exit 1
fi

# Extract filename without extension for executable
BASE_NAME=$(basename "$SOLUTION_FILE" .cpp)
EXE_FILE="./$BASE_NAME"

# Compile the solution
echo -e "${YELLOW}Compiling $SOLUTION_FILE...${NC}"
if ! $COMPILER -Wall -Wextra -g3 "$SOLUTION_FILE" -o "$EXE_FILE" 2>&1; then
    echo -e "${RED}Compilation failed!${NC}"
    exit 1
fi

echo -e "${GREEN}Compilation successful!${NC}"
echo ""

# Initialize counters
PASSED_TESTS=0
TOTAL_TESTS=0
FAILED_TESTS=()

# Count and run test cases
for input_file in "$TEST_CASES_DIR"/*.in; do
    if [ ! -f "$input_file" ]; then
        continue
    fi
    
    ((TOTAL_TESTS++))
    
    test_name=$(basename "$input_file" .in)
    output_file="$TEST_CASES_DIR/$test_name.out"
    
    echo -n "Test $test_name... "
    
    # Check if expected output file exists
    if [ ! -f "$output_file" ]; then
        echo -e "${YELLOW}SKIP (no expected output)${NC}"
        continue
    fi
    
    # Run the solution with input, timeout, and measure time
    start_time=$(date +%s%3N)
    
    # Use timeout with memory monitoring
    if timeout ${TIME_LIMIT}s /usr/bin/time -f "%M" "$EXE_FILE" < "$input_file" > temp_output.txt 2>temp_error.txt; then
        end_time=$(date +%s%3N)
        execution_time=$((end_time - start_time))
        actual_output=$(cat temp_output.txt)
        
        # Try to get memory usage (in KB)
        memory_kb=0
        if [ -f temp_error.txt ]; then
            memory_kb=$(tail -1 temp_error.txt 2>/dev/null || echo "0")
            memory_mb=$((memory_kb / 1024))
        fi
        
        rm -f temp_output.txt temp_error.txt
        
        # Check memory limit
        if [ $memory_mb -gt $MEMORY_LIMIT ]; then
            echo -e "${RED}MEMORY LIMIT EXCEEDED${NC} ${BLUE}(${execution_time}ms, ${memory_mb}MB)${NC}"
            FAILED_TESTS+=("$test_name")
            continue
        fi
        
        # Read expected output
        expected_output=$(cat "$output_file")
        
        # Normalize whitespace and compare
        actual_output=$(echo "$actual_output" | sed 's/[[:space:]]*$//')
        expected_output=$(echo "$expected_output" | sed 's/[[:space:]]*$//')
        
        # Handle empty output as "0" (valid answer when Odin cannot win)
        if [ -z "$actual_output" ]; then
            actual_output="0"
        fi
        
        # Check if actual output matches any of the expected valid answers
        is_valid=false
        
        # Split by lines for multiple test cases
        IFS=$'\n' read -rd '' -a expected_lines <<< "$expected_output"
        IFS=$'\n' read -rd '' -a actual_lines <<< "$actual_output"
        
        if [ ${#expected_lines[@]} -eq ${#actual_lines[@]} ]; then
            all_lines_valid=true
            for ((i=0; i<${#expected_lines[@]}; i++)); do
                expected_line="${expected_lines[i]}"
                # If expected line is empty, treat as "0"
                if [ -z "$expected_line" ]; then
                    expected_answers=("0")
                else
                    expected_answers=($expected_line)
                fi
                
                actual_answer="${actual_lines[i]// /}"
                
                line_valid=false
                for valid_answer in "${expected_answers[@]}"; do
                    if [ "$actual_answer" = "${valid_answer// /}" ]; then
                        line_valid=true
                        break
                    fi
                done
                
                if [ "$line_valid" = false ]; then
                    all_lines_valid=false
                    break
                fi
            done
            is_valid=$all_lines_valid
        fi
        
        if [ "$is_valid" = true ]; then
            echo -e "${GREEN}PASS${NC} ${BLUE}(${execution_time}ms)${NC}"
            ((PASSED_TESTS++))
        else
            echo -e "${RED}FAIL${NC}"
            echo -e "${RED}  Expected (any of): '$expected_output'${NC}"
            echo -e "${RED}  Actual:            '$actual_output'${NC}"
            FAILED_TESTS+=("$test_name")
        fi
    else
        end_time=$(date +%s%3N)
        execution_time=$((end_time - start_time))
        exit_code=$?
        
        # Try to get memory usage even on failure
        memory_kb=0
        memory_mb=0
        if [ -f temp_error.txt ]; then
            memory_kb=$(tail -1 temp_error.txt 2>/dev/null | grep -o '[0-9]*' || echo "0")
            memory_mb=$((memory_kb / 1024))
        fi
        
        if [ $exit_code -eq 124 ]; then
            echo -e "${RED}TIME LIMIT EXCEEDED${NC} ${BLUE}(${execution_time}ms)${NC}"
            if [ $memory_mb -gt 0 ]; then
                echo -e "${BLUE}  Memory used: ${memory_mb}MB${NC}"
            fi
        else
            echo -e "${RED}RUNTIME ERROR${NC} ${BLUE}(${execution_time}ms)${NC}"
            if [ $memory_mb -gt 0 ]; then
                echo -e "${BLUE}  Memory used: ${memory_mb}MB${NC}"
            fi
            if [ -f temp_output.txt ]; then
                actual_output=$(cat temp_output.txt)
                echo -e "${RED}  Error output: $actual_output${NC}"
            fi
        fi
        
        rm -f temp_output.txt temp_error.txt
        FAILED_TESTS+=("$test_name")
    fi
done

echo ""
echo -e "${BLUE}=== Test Results ===${NC}"
if [ $PASSED_TESTS -eq $TOTAL_TESTS ]; then
    echo -e "${GREEN}Passed: $PASSED_TESTS/$TOTAL_TESTS${NC}"
else
    echo -e "${YELLOW}Passed: $PASSED_TESTS/$TOTAL_TESTS${NC}"
fi

if [ ${#FAILED_TESTS[@]} -gt 0 ]; then
    echo -e "${RED}Failed Tests:${NC}"
    for failed_test in "${FAILED_TESTS[@]}"; do
        echo -e "${RED}  - $failed_test${NC}"
    done
fi

# Cleanup
rm -f "$EXE_FILE"

# Exit with appropriate code
if [ $PASSED_TESTS -eq $TOTAL_TESTS ]; then
    echo -e "${GREEN}All tests passed! ✓${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed! ✗${NC}"
    exit 1
fi