gcc -O2 -c deps/sqlite/sqlite3.c -o sqlite3.o
g++ -std=c++17 -O2 -o server.exe src/server/Server.cpp sqlite3.o -I./deps/sqlite -lws2_32
if ($?) {
    Write-Host "Build successful. Run .\server.exe to start the application."
} else {
    Write-Host "Build failed."
}
