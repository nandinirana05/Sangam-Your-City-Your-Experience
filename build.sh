#!/bin/bash
gcc -O2 -c deps/sqlite/sqlite3.c -o sqlite3.o
g++ -std=c++17 -O2 -o server src/server/Server.cpp sqlite3.o -I./deps/sqlite -lpthread
if [ $? -eq 0 ]; then
    echo "Build successful. Run ./server to start the application."
else
    echo "Build failed."
fi
