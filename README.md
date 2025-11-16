# Odin's Warrior Cascade - Competitive Programming Problem

This directory contains a complete competitive programming problem setup with automated testing infrastructure.

## 📁 Project Structure

- **qwen/**: Contains failed solution attempts by Qwen model (educational purposes)
- **test_cases/**: Input/output test cases (comprehensive test suite)
- **idea.md**: Problem development process and insights
- **problem.md**: Complete problem statement with examples
- **solution.md**: Detailed solution explanation with chain reaction analysis
- **solution.cpp**: Optimal O(n log n) accepted solution
- **requirements.json**: Time (4s) and memory (512MB) constraints
- **solution_bf.cpp**: Brute-force solution for comparison
- **generator.cpp**: Test case generator framework

## 🚀 Quick Start

1. Read the problem statement in `problem.md`
2. Try to solve it yourself
3. Check your approach against `solution.md`
4. Test your solution using the automated test runners

## 🔧 Building and Testing

### Automated Test Runners

This project includes multiple test runner scripts with comprehensive features:

#### PowerShell Script (Windows) - **Recommended**

```powershell
# Test the optimal solution
.\test_runner.ps1 solution.cpp

# Test specific solutions
.\test_runner.ps1 qwen\solution_01.cpp
.\test_runner.ps1 solution_bf.cpp

# Custom test directory
.\test_runner.ps1 solution.cpp test_cases
```

#### Batch Script (Windows Command Prompt)

```cmd
test_runner.bat solution.cpp
test_runner.bat qwen\solution_01.cpp
```

#### Bash Script (Linux/macOS)

```bash
chmod +x test_runner.sh
./test_runner.sh solution.cpp
./test_runner.sh qwen/solution_01.cpp
```

### Manual Compilation

```bash
# Using VS Code task (Build C++ file)
Ctrl+Shift+P -> "Tasks: Run Task" -> "Build C++ file"

# Manual compilation
g++ -Wall -Wextra -g3 solution.cpp -o solution.exe

# Run against specific test case
.\solution.exe < test_cases\1.in
```

## 🧪 Test Runner Features

### Comprehensive Testing
- **Automatic compilation** with error handling
- **Time limit enforcement** (4 seconds per test case)
- **Memory limit monitoring** (512MB)
- **Detailed output comparison** with diff visualization
- **Colored output** (Pass/Fail/TLE/MLE)
- **Performance metrics** (execution time per test)
- **Multiple answer validation** (supports brute-force generated answers)

### Example Output

```
=== C++ Solution Test Runner ===
Solution: solution.cpp
Test Cases Directory: test_cases
Requirements: 4000ms timeout, 512MB memory limit

Compiling solution.cpp...
Compilation successful!

Running 8 test cases...

Test 1... PASS (15ms)
Test 2... PASS (12ms)
Test 3... PASS (847ms)
Test 4... TLE (>4000ms)
Test 5... PASS (1205ms)

=== Test Results ===
Passed: 4/5 tests
Failed Tests:
  - Test 4: Time Limit Exceeded (TLE)

Some tests failed! ✗
```

## 📊 Solution Variants

### 1. Optimal Solution (`solution.cpp`)
- **Algorithm**: Greedy with DFS timing and interval-based chain simulation
- **Time Complexity**: O(n log n)
- **Space Complexity**: O(n)
- **Status**: Passes all test cases

### 2. Brute Force (`solution_bf.cpp`)
- **Algorithm**: Recursive game tree exploration
- **Time Complexity**: O(n³)
- **Purpose**: Generate all valid answers, verify correctness

### 3. Qwen Attempts (`qwen/`)
- **solution_01.cpp**: Sophisticated but flawed (TLE on large cases)
- **solution_02.cpp**: Multiple failure modes (WA, TLE, RE)
- **solution_03.cpp**: Additional failed approach
- **Purpose**: Educational - demonstrates common competitive programming mistakes

## 🔍 Test Case Structure

```
test_cases/
├── 1.in     # Sample input from problem statement
├── 1.out    # Expected output
├── 2.in     # Edge case: single node
├── 2.out    # Expected: 0 (no valid moves)
├── 3.in     # Small tree with multiple valid answers
├── 3.out    # One of the valid answers
├── 4.in     # Medium complexity
├── 4.out    # Expected output
├── 5.in     # Large case (stress test)
├── 5.out    # Expected output
└── ...
```

## 🐛 Troubleshooting

### Compilation Issues
- Ensure MinGW/GCC is installed and in PATH
- Check C++ syntax and missing headers
- Verify compiler version supports C++17 features

### Runtime Issues
- **Time Limit Exceeded**: Optimize algorithm complexity
- **Memory Limit Exceeded**: Reduce memory usage
- **Wrong Answer**: Check edge cases and logic
- **Runtime Error**: Handle array bounds and null pointers

### Path Issues
- Use absolute paths if relative paths fail
- Ensure `test_cases/` directory exists
- Check file permissions on Unix systems

## 📈 Performance Analysis

| Solution | Time Complexity | Space | Test Results |
|----------|----------------|-------|-------------|
| Optimal | O(n log n) | O(n) | 8/8 PASS |
| Brute Force | O(n³) | O(n) | 6/8 PASS (TLE on large) |
| Qwen-01 | O(n² log n) | O(n) | 7/8 PASS (TLE on largest) |
| Qwen-02 | O(n³) | O(n²) | 3/8 PASS (Multiple issues) |

## 🎯 Testing All Solutions

```powershell
# Quick test all variants
.\test_runner.ps1 solution.cpp         # Should pass all
.\test_runner.ps1 solution_bf.cpp      # Should pass most (TLE on large)
.\test_runner.ps1 qwen\solution_01.cpp # Should show realistic TLE
.\test_runner.ps1 qwen\solution_02.cpp # Should show multiple failure modes
.\test_runner.ps1 qwen\solution_03.cpp # Additional failed approach
```

## 📚 Educational Value

- **Algorithm Design**: Learn advanced techniques (DFS timing, interval arithmetic, game theory)
- **Optimization**: Understand the progression from O(n³) to O(n log n)
- **Debugging**: Analyze failed solutions to understand common pitfalls
- **Testing**: Experience comprehensive test-driven development
- **Chain Reaction Modeling**: Complex state transitions and mathematical modeling

## 🏆 Competition Tips

1. **Read carefully**: Understand the chain reaction mechanism
2. **Start simple**: Implement brute force first for small cases
3. **Optimize incrementally**: Identify bottlenecks and improve
4. **Test thoroughly**: Use provided test runners for validation
5. **Handle edge cases**: Single nodes, equal strengths, cycles

---

**Note**: The Qwen model attempts are intentionally flawed for educational purposes, demonstrating realistic competitive programming mistakes and optimization challenges.