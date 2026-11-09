# Step 1: Compile SQLite (C library)
gcc -O2 -c deps/sqlite/sqlite3.c -o sqlite3.o
if (-not $?) { Write-Host "SQLite compile failed."; exit 1 }

# Step 2: Compile the Sangam C++ core
g++ -std=c++17 -O2 -o sangam_core.exe src/cli/main.cpp sqlite3.o -I./deps/sqlite
if ($?) {
    Write-Host "Build successful. Run: python server.py"
} else {
    Write-Host "Build failed."
}
