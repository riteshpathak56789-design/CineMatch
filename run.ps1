# CineMatch PowerShell Build & Run Helper
param (
    [switch]$Test,
    [switch]$Clean,
    [switch]$BuildOnly,
    [switch]$NoColor
)

$env:PATH = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;$env:PATH"

if ($Clean) {
    Write-Host "Cleaning CineMatch build artifacts..." -ForegroundColor Yellow
    Remove-Item -Path "cinematch.exe", "test_runner.exe", "*.o" -Force -ErrorAction SilentlyContinue
    Write-Host "Clean complete." -ForegroundColor Green
    exit 0
}

if ($Test) {
    Write-Host "Compiling test suite..." -ForegroundColor Cyan
    & gcc -std=c11 -Wall -Wextra -pedantic -Iinclude tests/test_runner.c tests/test_ds.c tests/test_json.c tests/test_tmdb.c src/movie.c src/ds.c src/json_parser.c src/tmdb.c src/ui.c -lcurl -o test_runner.exe
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Test build failed with exit code $LASTEXITCODE" -ForegroundColor Red
        exit $LASTEXITCODE
    }
    Write-Host "Running tests..." -ForegroundColor Green
    & .\test_runner.exe
    exit $LASTEXITCODE
}

Write-Host "Compiling CineMatch application..." -ForegroundColor Cyan
& gcc -std=c11 -Wall -Wextra -pedantic -Iinclude src/main.c src/movie.c src/ds.c src/json_parser.c src/tmdb.c src/ui.c -lcurl -o cinematch.exe
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed with exit code $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}
Write-Host "Build successful: cinematch.exe" -ForegroundColor Green

if (-not $BuildOnly) {
    $argsList = @()
    if ($NoColor) { $argsList += "--no-color" }
    & .\cinematch.exe @argsList
}
