# PowerShell Test Runner Script with Requirements Enforcement
# Usage: .\test_runner.ps1 [solution_file] [test_cases_dir]
# Example: .\test_runner.ps1 solution.cpp test_cases

param(
    [string]$SolutionFile = "solution.cpp",
    [string]$TestCasesDir = "test_cases",
    [string]$Compiler = "C:\MinGW\bin\g++.exe",
    [string]$RequirementsFile = "requirements.json"
)

# Colors for output
$Red = "Red"
$Green = "Green"
$Yellow = "Yellow"
$Blue = "Cyan"

# Load requirements if file exists
$TimeLimit = 4
$MemoryLimit = 512
if (Test-Path $RequirementsFile) {
    try {
        $Requirements = Get-Content $RequirementsFile | ConvertFrom-Json
        $TimeLimit = $Requirements.time
        $MemoryLimit = $Requirements.space
        Write-Host "Loaded requirements: ${TimeLimit}s time, ${MemoryLimit}MB memory" -ForegroundColor $Blue
    } catch {
        Write-Host "Warning: Could not parse requirements.json, using defaults" -ForegroundColor $Yellow
    }
}

Write-Host "=== C++ Solution Test Runner ===" -ForegroundColor $Blue
Write-Host "Solution: $SolutionFile" -ForegroundColor $Blue
Write-Host "Test Cases Directory: $TestCasesDir" -ForegroundColor $Blue
Write-Host "Time Limit: ${TimeLimit}s, Memory Limit: ${MemoryLimit}MB" -ForegroundColor $Blue
Write-Host ""

# Check if solution file exists
if (!(Test-Path $SolutionFile)) {
    Write-Host "Error: Solution file '$SolutionFile' not found!" -ForegroundColor $Red
    exit 1
}

# Check if test cases directory exists
if (!(Test-Path $TestCasesDir)) {
    Write-Host "Error: Test cases directory '$TestCasesDir' not found!" -ForegroundColor $Red
    exit 1
}

# Extract filename without extension for executable
$BaseName = [System.IO.Path]::GetFileNameWithoutExtension($SolutionFile)
$ExeFile = "$BaseName.exe"

# Compile the solution
Write-Host "Compiling $SolutionFile..." -ForegroundColor $Yellow
$CompileResult = & $Compiler -Wall -Wextra -g3 $SolutionFile -o $ExeFile 2>&1

if ($LASTEXITCODE -ne 0) {
    Write-Host "Compilation failed!" -ForegroundColor $Red
    Write-Host $CompileResult -ForegroundColor $Red
    exit 1
}

Write-Host "Compilation successful!" -ForegroundColor $Green
Write-Host ""

# Find all input test files
$InputFiles = Get-ChildItem -Path $TestCasesDir -Filter "*.in" | Sort-Object Name

if ($InputFiles.Count -eq 0) {
    Write-Host "No test cases found in $TestCasesDir!" -ForegroundColor $Red
    exit 1
}

$PassedTests = 0
$TotalTests = $InputFiles.Count
$FailedTests = @()

Write-Host "Running $TotalTests test cases..." -ForegroundColor $Blue
Write-Host ""

foreach ($InputFile in $InputFiles) {
    $TestName = [System.IO.Path]::GetFileNameWithoutExtension($InputFile.Name)
    $OutputFile = Join-Path $TestCasesDir "$TestName.out"
    
    Write-Host "Test $TestName... " -NoNewline
    
    # Check if expected output file exists
    if (!(Test-Path $OutputFile)) {
        Write-Host "SKIP (no expected output)" -ForegroundColor $Yellow
        continue
    }
    
    # Run the solution with input and enforce time limit
    try {
        # Read input file content
        $InputContent = Get-Content $InputFile.FullName -Raw
        
        # Run the solution with input using proper PowerShell method
        $StartTime = Get-Date
        
        $ProcessInfo = New-Object System.Diagnostics.ProcessStartInfo
        $ProcessInfo.FileName = ".\$ExeFile"
        $ProcessInfo.RedirectStandardInput = $true
        $ProcessInfo.RedirectStandardOutput = $true
        $ProcessInfo.RedirectStandardError = $true
        $ProcessInfo.UseShellExecute = $false
        $ProcessInfo.CreateNoWindow = $true
        
        $Process = New-Object System.Diagnostics.Process
        $Process.StartInfo = $ProcessInfo
        $Process.Start() | Out-Null
        
        # Send input to the process
        $Process.StandardInput.Write($InputContent)
        $Process.StandardInput.Close()
        
        # Wait with timeout
        $TimeoutMs = $TimeLimit * 1000
        $TimedOut = $false
        if (-not $Process.WaitForExit($TimeoutMs)) {
            $Process.Kill()
            $TimedOut = $true
        }
        
        $EndTime = Get-Date
        $ExecutionTime = ($EndTime - $StartTime).TotalMilliseconds
        $ActualOutput = $Process.StandardOutput.ReadToEnd()
        $ErrorOutput = $Process.StandardError.ReadToEnd()
        
        # Get memory usage (approximate from process)
        $MemoryUsageMB = 0
        try {
            if (-not $TimedOut -and $Process.HasExited -eq $false) {
                $MemoryUsageMB = [Math]::Round($Process.WorkingSet64 / 1MB, 2)
            }
        } catch {
            # Memory info not available
        }
        
        if ($TimedOut) {
            Write-Host "TIME LIMIT EXCEEDED" -ForegroundColor $Red -NoNewline
            Write-Host " (${ExecutionTime}ms)" -ForegroundColor $Blue
            if ($MemoryUsageMB -gt 0) {
                Write-Host "  Memory used: ${MemoryUsageMB}MB" -ForegroundColor $Blue
            }
            $FailedTests += @{Name=$TestName; Reason="Time Limit Exceeded"; Time=$ExecutionTime}
            continue
        }
        
        if ($Process.ExitCode -ne 0) {
            Write-Host "RUNTIME ERROR" -ForegroundColor $Red -NoNewline
            Write-Host " (${ExecutionTime}ms)" -ForegroundColor $Blue
            if ($MemoryUsageMB -gt 0) {
                Write-Host "  Memory used: ${MemoryUsageMB}MB" -ForegroundColor $Blue
            }
            Write-Host "  Error output: $ErrorOutput" -ForegroundColor $Red
            $FailedTests += @{Name=$TestName; Reason="Runtime Error"; Output=$ErrorOutput; Time=$ExecutionTime}
            continue
        }
        
        # Check memory limit
        if ($MemoryUsageMB -gt $MemoryLimit) {
            Write-Host "MEMORY LIMIT EXCEEDED" -ForegroundColor $Red -NoNewline
            Write-Host " (${ExecutionTime}ms, ${MemoryUsageMB}MB)" -ForegroundColor $Blue
            $FailedTests += @{Name=$TestName; Reason="Memory Limit Exceeded"; Time=$ExecutionTime; Memory=$MemoryUsageMB}
            continue
        }
        
        # Read expected output
        $ExpectedOutput = Get-Content $OutputFile -Raw
        if ($ExpectedOutput -eq $null) { $ExpectedOutput = "" }
        
        # Normalize whitespace and compare
        $ActualOutput = $ActualOutput.Trim() -replace "`r`n", "`n"
        $ExpectedOutput = $ExpectedOutput.Trim() -replace "`r`n", "`n"
        
        # Handle empty output as "0" (valid answer when Odin cannot win)
        if ([string]::IsNullOrWhiteSpace($ActualOutput)) {
            $ActualOutput = "0"
        }
        
        # Check if actual output matches any of the expected valid answers
        $IsValidAnswer = $false
        
        # Split expected output by lines (for multiple test cases)
        $ExpectedLines = if ([string]::IsNullOrWhiteSpace($ExpectedOutput)) { @("") } else { $ExpectedOutput -split "`n" }
        $ActualLines = if ([string]::IsNullOrWhiteSpace($ActualOutput)) { @("0") } else { $ActualOutput -split "`n" }
        
        if ($ExpectedLines.Count -eq $ActualLines.Count) {
            $AllLinesValid = $true
            for ($i = 0; $i -lt $ExpectedLines.Count; $i++) {
                $ExpectedLine = if ([string]::IsNullOrWhiteSpace($ExpectedLines[$i])) { "0" } else { $ExpectedLines[$i].ToString().Trim() }
                $ExpectedAnswers = @($ExpectedLine -split "\s+" | Where-Object { $_ -ne "" })
                $ActualAnswer = $ActualLines[$i].ToString().Trim()
                
                # If no expected answers, treat as "0"
                if ($ExpectedAnswers.Count -eq 0) {
                    $ExpectedAnswers = @("0")
                }
                
                # Check if actual answer is in the list of valid expected answers
                $LineValid = $false
                foreach ($ValidAnswer in $ExpectedAnswers) {
                    $ValidAnswerStr = $ValidAnswer.ToString().Trim()
                    if ($ActualAnswer -eq $ValidAnswerStr) {
                        $LineValid = $true
                        break
                    }
                }
                
                if (-not $LineValid) {
                    $AllLinesValid = $false
                    break
                }
            }
            $IsValidAnswer = $AllLinesValid
        }
        
        if ($IsValidAnswer) {
            Write-Host "PASS" -ForegroundColor $Green -NoNewline
            Write-Host " (${ExecutionTime}ms)" -ForegroundColor $Blue
            $PassedTests++
        } else {
            Write-Host "FAIL" -ForegroundColor $Red
            Write-Host "  Expected (any of): '$ExpectedOutput'" -ForegroundColor $Red
            Write-Host "  Actual:            '$ActualOutput'" -ForegroundColor $Red
            $FailedTests += @{Name=$TestName; Expected=$ExpectedOutput; Actual=$ActualOutput}
        }
    }
    catch {
        Write-Host "ERROR" -ForegroundColor $Red
        Write-Host "Exception: $($_.Exception.Message)" -ForegroundColor $Red
        $FailedTests += @{Name=$TestName; Reason="Exception"; Error=$_.Exception.Message}
    }
}

Write-Host ""
Write-Host "=== Test Results ===" -ForegroundColor $Blue
Write-Host "Passed: $PassedTests/$TotalTests" -ForegroundColor $(if ($PassedTests -eq $TotalTests) { $Green } else { $Yellow })

if ($FailedTests.Count -gt 0) {
    Write-Host "Failed Tests:" -ForegroundColor $Red
    foreach ($Failed in $FailedTests) {
        Write-Host "  - $($Failed.Name): $($Failed.Reason)" -ForegroundColor $Red
    }
}

# Cleanup
try {
    if (Test-Path $ExeFile) {
        Start-Sleep -Milliseconds 100  # Brief pause to ensure process cleanup
        Remove-Item $ExeFile -Force
    }
} catch {
    # Ignore cleanup errors
}

# Exit with appropriate code
if ($PassedTests -eq $TotalTests) {
    Write-Host "All tests passed!" -ForegroundColor $Green
    exit 0
} else {
    Write-Host "Some tests failed!" -ForegroundColor $Red
    exit 1
}