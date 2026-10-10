# Sangam – Your City, Your Experience

Sangam is a robust tourism web application built for exploring cities, mapping tourist attractions, and optimizing travel itineraries. This project serves as an academic demonstration of Data Structures and Algorithms (DSA) built seamlessly into a web product.

## Key Features
- **Interactive Map:** Powered by Leaflet.js and responsive UI aesthetics.
- **Lightning-Fast Search:** Prefix autocompletion using a custom C++ **Trie**.
- **Optimal Routing:** C++ backend computes the shortest paths utilizing **Dijkstra's Algorithm**.
- **Smart Itineraries:** Solves the Multi-stop TSP (Travelling Salesperson Problem) via **Nearest Neighbour** to find the best route.
- **Persistence:** SQLite integration ensures your favorite places and itineraries are safely saved.

## Architecture
- **Frontend:** HTML, CSS, JavaScript
- **Backend Bridge:** Python 3 HTTP Server (`server.py`)
- **Core Algorithms:** C++ CLI application (`sangam_core.exe`)
- **Database:** SQLite3

*See `docs/architecture.md` for a complete diagram.*

## Prerequisites
- **Windows:** PowerShell, Python 3, `gcc`/`g++` (MinGW)
- **macOS/Linux:** Terminal, Python 3, `gcc`/`g++`

## Installation and Startup

### Windows
1. Build the C++ core engine:
   ```powershell
   .\build_cli.ps1
   ```
2. Start the Python server:
   ```powershell
   python server.py
   ```
3. Open `http://localhost:5050` in your web browser.

### macOS / Linux
1. Build the C++ core engine:
   ```bash
   chmod +x build_cli.sh
   ./build_cli.sh
   ```
2. Start the Python server:
   ```bash
   python3 server.py
   ```
3. Open `http://localhost:5050` in your web browser.

## Documentation
- `docs/integration-audit.md` - Analysis of the codebase before integration.
- `docs/architecture.md` - Architectural overview.
- `docs/integration-status.md` - Status of the 9 project modules.
