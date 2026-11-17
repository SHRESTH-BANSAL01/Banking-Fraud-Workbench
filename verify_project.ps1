
Write-Host ""
Write-Host "============================================"
Write-Host " BANKING FRAUD SYSTEM - PROJECT VERIFICATION"
Write-Host "============================================"

# Stop if a PowerShell command fails
$ErrorActionPreference = "Stop"

# Make sure we run from the project root
Set-Location $PSScriptRoot

# Create report directory
New-Item -ItemType Directory -Force reports | Out-Null

# Collect all C source files
$files = @(
    Get-ChildItem -Path src -Recurse -Filter *.c |
    ForEach-Object { $_.FullName }
)

if ($files.Count -eq 0) {
    Write-Host "No C source files found."
    exit 1
}

Write-Host ""
Write-Host "Compiling project..."

gcc -std=c11 -O2 -Wall -Wextra -Iinclude $files -o fraud.exe

if ($LASTEXITCODE -ne 0) {
    Write-Host "Compilation failed."
    exit 1
}

Write-Host "Compilation successful."

# Run each verification mode
$modes = @(
    "--test",
    "--benchmark",
    "--stress",
    "--compare",
    "--complexity"
)

foreach ($mode in $modes) {
    Write-Host ""
    Write-Host "============================================"
    Write-Host "Running: $mode"
    Write-Host "============================================"

    & .\fraud.exe $mode

    if ($LASTEXITCODE -ne 0) {
        Write-Host "FAILED: $mode"
        exit 1
    }

    Write-Host "PASSED: $mode"
}

Write-Host ""
Write-Host "============================================"
Write-Host " ALL PROJECT VERIFICATION MODES PASSED"
Write-Host "============================================"

Write-Host ""
Write-Host "Generated reports:"
Get-ChildItem reports -Filter *.csv
