gcc -O2 -c deps/sqlite/sqlite3.c -o sqlite3.o
g++ -std=c++17 -O2 -o sangam_core.exe src/cli/main.cpp sqlite3.o -I./deps/sqlite
if ($?) {
    Write-Host "Build successful."
} else {
    Write-Host "Build failed."
}
