# Integration Audit

## 1. Existing Architecture & Flaws
- **Separation of Concerns:** The project was built in isolated silos. Module 1 contained `graph_construction.cpp`, Module 2 contained `route_engine.cpp` & `graph.h`, Module 7/8/9 relied on `vectors.cpp` to read `sangam.db`.
- **Frontend Disconnect:** `index.html` contained mock calculation logic (straight-line `km()` function) instead of communicating with the C++ algorithm backend. 
- **Missing API Layer:** There was a `server.exe` and `httplib.h` present in the `webpage/` folder, but no C++ source code binding the `cpp-httplib` server to the modules. 
- **Database Alignment:** The SQLite database schema (`schema.sql`) defined places and connections, but the graph code read from CSVs (`places.csv` and `roads.csv`).

## 2. Integration Plan
- Extract common data types into `core/Models.h`.
- Centralize Graph structure into `core/Graph.h` and routing algorithms (Dijkstra, NN) into `core/RouteEngine.h`.
- Implement `core/Trie.h` to satisfy the missing autocomplete requirement.
- Wrap the SQLite database access in `core/Database.h` using `sqlite3.h`.
- Create a lightweight `cli/main.cpp` that loads the database, runs the algorithms, and outputs JSON.
- Implement a bridge `server.py` to serve the static frontend and proxy HTTP requests to the CLI.
- Modify `index.html` to consume `http://localhost:5050/api/...` instead of computing routes locally.
