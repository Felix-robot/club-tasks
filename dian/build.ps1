# PowerShell build script: compile with gcc (mingw-w64)
# Usage:
#   ./build.ps1

$ErrorActionPreference = 'Stop'

# Ensure gcc exists
$gcc = Get-Command gcc -ErrorAction SilentlyContinue
if (-not $gcc) {
	Write-Error 'gcc not found. Please install mingw-w64 and ensure gcc is in PATH.'
}

# Create output folders (same as source directories)
New-Item -ItemType Directory -Force -Path 'test1' | Out-Null
New-Item -ItemType Directory -Force -Path 'test2' | Out-Null

# Build test1
Write-Host 'Building test1...'
& gcc -O2 -Wall -o 'test1/test1.exe' 'test1/main.c'
if ($LASTEXITCODE -ne 0) { throw 'Build test1 failed' }

# Build test2
Write-Host 'Building test2...'
& gcc -O2 -Wall -o 'test2/test2.exe' 'test2/main.c'
if ($LASTEXITCODE -ne 0) { throw 'Build test2 failed' }

Write-Host 'Build completed. Executables: test1/test1.exe, test2/test2.exe'
