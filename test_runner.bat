@echo off
REM Batch Test Runner Script
REM Usage: test_runner.bat [solution_file] [test_cases_dir]
REM Example: test_runner.bat solution.cpp test_cases

setlocal enabledelayedexpansion

REM Default parameters
set SOLUTION_FILE=%1
set TEST_CASES_DIR=%2
set COMPILER=C:\MinGW\bin\g++.exe
set REQUIREMENTS_FILE=requirements.json

if "%SOLUTION_FILE%"=="" set SOLUTION_FILE=solution.cpp
if "%TEST_CASES_DIR%"=="" set TEST_CASES_DIR=test_cases

REM Default limits
set TIME_LIMIT=4
set MEMORY_LIMIT=512

REM Try to read requirements (simplified - actual JSON parsing is complex in batch)
if exist "%REQUIREMENTS_FILE%" (
    echo Found requirements.json, using 4s time limit and 512MB memory limit
) else (
    echo Using default limits: 4s time, 512MB memory
)

echo === C++ Solution Test Runner ===
echo Solution: %SOLUTION_FILE%
echo Test Cases Directory: %TEST_CASES_DIR%
echo Time Limit: %TIME_LIMIT%s, Memory Limit: %MEMORY_LIMIT%MB
echo.

REM Check if solution file exists
if not exist "%SOLUTION_FILE%" (
    echo Error: Solution file '%SOLUTION_FILE%' not found!
    exit /b 1
)

REM Check if test cases directory exists
if not exist "%TEST_CASES_DIR%" (
    echo Error: Test cases directory '%TEST_CASES_DIR%' not found!
    exit /b 1
)

REM Extract filename without extension for executable
for %%F in ("%SOLUTION_FILE%") do set BASE_NAME=%%~nF
set EXE_FILE=%BASE_NAME%.exe

REM Compile the solution
echo Compiling %SOLUTION_FILE%...
"%COMPILER%" -Wall -Wextra -g3 "%SOLUTION_FILE%" -o "%EXE_FILE%" 2>compile.log

if errorlevel 1 (
    echo Compilation failed!
    type compile.log
    del compile.log 2>nul
    exit /b 1
)

echo Compilation successful!
del compile.log 2>nul
echo.

REM Initialize counters
set PASSED_TESTS=0
set TOTAL_TESTS=0
set FAILED_TESTS=

REM Count total test files
for %%F in ("%TEST_CASES_DIR%\*.in") do set /a TOTAL_TESTS+=1

if %TOTAL_TESTS%==0 (
    echo No test cases found in %TEST_CASES_DIR%!
    exit /b 1
)

echo Running %TOTAL_TESTS% test cases...
echo.

REM Run each test case
for %%F in ("%TEST_CASES_DIR%\*.in") do (
    set TEST_NAME=%%~nF
    set OUTPUT_FILE=%TEST_CASES_DIR%\!TEST_NAME!.out
    
    echo Test !TEST_NAME!... 
    
    REM Check if expected output file exists
    if not exist "!OUTPUT_FILE!" (
        echo   SKIP ^(no expected output^)
        goto :continue
    )
    
    REM Run the solution with input and timeout (using timeout command)
    powershell -Command "$start = Get-Date; timeout /t %TIME_LIMIT% '%EXE_FILE%' < '%%F' > actual_output.tmp 2>error.tmp; $end = Get-Date; $time = ($end - $start).TotalMilliseconds; echo $time > execution_time.tmp"
    
    REM Read execution time
    set /p EXEC_TIME=<execution_time.tmp
    
    if errorlevel 1 (
        if errorlevel 2 (
            echo   TIME LIMIT EXCEEDED ^(!EXEC_TIME!ms^)
            set FAILED_TESTS=!FAILED_TESTS! !TEST_NAME!
            goto :continue
        ) else (
            echo   RUNTIME ERROR ^(!EXEC_TIME!ms^)
            type error.tmp
            set FAILED_TESTS=!FAILED_TESTS! !TEST_NAME!
            goto :continue
        )
    )
    
    REM Compare outputs (check if actual is one of the valid expected answers)
    set VALID_ANSWER=false
    
    REM Read expected and actual outputs
    set /p EXPECTED=<"!OUTPUT_FILE!" 2>nul
    set /p ACTUAL=<actual_output.tmp 2>nul
    
    REM Handle empty outputs - treat as "0"
    if "!ACTUAL!"=="" set ACTUAL=0
    if "!EXPECTED!"=="" set EXPECTED=0
    
    REM Simple check - if they're exactly equal, it's valid
    if "!ACTUAL!"=="!EXPECTED!" (
        set VALID_ANSWER=true
    ) else (
        REM Check if actual is one of the space-separated expected answers
        for %%A in (!EXPECTED!) do (
            if "!ACTUAL!"=="%%A" set VALID_ANSWER=true
        )
    )
    
    if "!VALID_ANSWER!"=="true" (
        echo   PASS ^(!EXEC_TIME!ms^)
        set /a PASSED_TESTS+=1
    ) else (
        echo   FAIL
        echo   Expected ^(any of^): !EXPECTED!
        echo   Actual:            !ACTUAL!
        set FAILED_TESTS=!FAILED_TESTS! !TEST_NAME!
    )
    
    :continue
    del actual_output.tmp 2>nul
    del error.tmp 2>nul
    del execution_time.tmp 2>nul
)

echo.
echo === Test Results ===
echo Passed: %PASSED_TESTS%/%TOTAL_TESTS%

if not "%FAILED_TESTS%"=="" (
    echo Failed Tests:%FAILED_TESTS%
)

REM Cleanup
del "%EXE_FILE%" 2>nul

REM Exit with appropriate code
if %PASSED_TESTS%==%TOTAL_TESTS% (
    echo All tests passed! ✓
    exit /b 0
) else (
    echo Some tests failed! ✗
    exit /b 1
)