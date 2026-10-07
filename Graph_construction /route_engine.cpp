// Module 2: Route & Itinerary Optimization (Ishita Bijalwan)
// Uses the Graph from graph.h (Module 1).
//
//  1. MinHeap        - hand-written priority queue (binary heap)
//  2. Dijkstra       - shortest path (distance or time)
//  3. A*             - same, guided by a straight-line (Haversine) heuristic
//  4. planItinerary  - best visiting order for several stops
//                      (nearest-neighbour + 2-opt improvement)
//
// Compile: g++ -std=c++17 -O2 route_engine.cpp -o route
// Run:     ./route

#include "graph.h"
#include <limits>
#include <algorithm>
#include <utility>

const double INF = numeric_limits<double>::infinity();
const double SPEED_KMH = 30.0;   // must match the default speed in Graph::addRoad

enum class Metric { DISTANCE, TIME };

inline double edgeCost(const Edge& e, Metric m) {
    return m == Metric::DISTANCE ? e.distanceKm : e.timeMin;
}

// ======================= 1. Priority Queue (Min-Heap) =======================

class MinHeap {
    vector<pair<double, int>> h;   // (priority, node id)

    void siftUp(int i) {
        while (i > 0) {
            int p = (i - 1) / 2;
            if (h[p].first <= h[i].first) break;
            swap(h[p], h[i]);
            i = p;
        }
    }
    void siftDown(int i) {
        int n = (int)h.size();
        while (true) {
            int l = 2 * i + 1, r = l + 1, m = i;
            if (l < n && h[l].first < h[m].first) m = l;
            if (r < n && h[r].first < h[m].first) m = r;
            if (m == i) break;
            swap(h[m], h[i]);
            i = m;
        }
    }
public:
    bool empty() const { return h.empty(); }
    int size() const { return (int)h.size(); }
    void push(double priority, int node) {
        h.push_back({priority, node});
        siftUp((int)h.size() - 1);
    }
    pair<double, int> pop() {          // removes and returns the smallest
        pair<double, int> top = h[0];
        h[0] = h.back();
        h.pop_back();
        if (!h.empty()) siftDown(0);
        return top;
    }
};

// ============================== Route result ==============================

struct Route {
    bool found = false;
    vector<int> path;        // place ids from source to destination
    double totalKm = 0;
    double totalMin = 0;
    int nodesExplored = 0;   // to compare Dijkstra vs A*
};

// Cheapest edge u -> v (there is normally exactly one)
const Edge* findEdge(const Graph& g, int u, int v, Metric m) {
    const Edge* best = nullptr;
    for (const Edge& e : g.neighbors(u))
        if (e.to == v && (!best || edgeCost(e, m) < edgeCost(*best, m))) best = &e;
    return best;
}

void fillTotals(const Graph& g, Route& r, Metric m) {
    r.totalKm = r.totalMin = 0;
    for (size_t i = 0; i + 1 < r.path.size(); i++) {
        const Edge* e = findEdge(g, r.path[i], r.path[i + 1], m);
        r.totalKm += e->distanceKm;
        r.totalMin += e->timeMin;
    }
}

vector<int> buildPath(const vector<int>& prev, int src, int dst) {
    vector<int> path;
    for (int v = dst; v != -1; v = prev[v]) path.push_back(v);
    reverse(path.begin(), path.end());
    if (path.empty() || path[0] != src) return {};
    return path;
}

// ============================== 2. Dijkstra ==============================
// Fills dist[] and prev[] from src. If target >= 0, stops once target is final.

void dijkstraAll(const Graph& g, int src, Metric m,
                 vector<double>& dist, vector<int>& prev,
                 int target = -1, int* explored = nullptr) {
    int n = g.size();
    dist.assign(n, INF);
    prev.assign(n, -1);
    vector<bool> done(n, false);
    MinHeap pq;
    int count = 0;

    dist[src] = 0;
    pq.push(0, src);
    while (!pq.empty()) {
        int u = pq.pop().second;
        if (done[u]) continue;           // stale heap entry
        done[u] = true;
        count++;
        if (u == target) break;
        for (const Edge& e : g.neighbors(u)) {
            double nd = dist[u] + edgeCost(e, m);
            if (nd < dist[e.to]) {
                dist[e.to] = nd;
                prev[e.to] = u;
                pq.push(nd, e.to);
            }
        }
    }
    if (explored) *explored = count;
}

Route dijkstra(const Graph& g, int src, int dst, Metric m = Metric::DISTANCE) {
    Route r;
    vector<double> dist;
    vector<int> prev;
    dijkstraAll(g, src, m, dist, prev, dst, &r.nodesExplored);
    if (dist[dst] == INF) return r;
    r.path = buildPath(prev, src, dst);
    r.found = !r.path.empty();
    if (r.found) fillTotals(g, r, m);
    return r;
}

// ================================ 3. A* ===================================
// f(n) = g(n) + h(n), h = straight-line distance to goal (never overestimates
// as long as road distances are >= straight-line distance).

double heuristic(const Graph& g, int u, int goal, Metric m) {
    const Place& a = g.getPlace(u);
    const Place& b = g.getPlace(goal);
    double km = Graph::haversine(a.lat, a.lon, b.lat, b.lon);
    return m == Metric::DISTANCE ? km : (km / SPEED_KMH) * 60.0;
}

Route aStar(const Graph& g, int src, int dst, Metric m = Metric::DISTANCE) {
    Route r;
    int n = g.size();
    vector<double> gScore(n, INF);
    vector<int> prev(n, -1);
    vector<bool> closed(n, false);
    MinHeap pq;

    gScore[src] = 0;
    pq.push(heuristic(g, src, dst, m), src);
    while (!pq.empty()) {
        int u = pq.pop().second;
        if (closed[u]) continue;
        closed[u] = true;
        r.nodesExplored++;
        if (u == dst) break;
        for (const Edge& e : g.neighbors(u)) {
            double ng = gScore[u] + edgeCost(e, m);
            if (ng < gScore[e.to]) {
                gScore[e.to] = ng;
                prev[e.to] = u;
                pq.push(ng + heuristic(g, e.to, dst, m), e.to);
            }
        }
    }
    if (gScore[dst] == INF) return r;
    r.path = buildPath(prev, src, dst);
    r.found = !r.path.empty();
    if (r.found) fillTotals(g, r, m);
    return r;
}

// ========================= 4. Itinerary optimization =======================

struct Itinerary {
    bool found = false;
    vector<int> order;       // stops in visiting order (starts with start)
    vector<int> fullPath;    // every place passed through, in order
    double totalKm = 0;
    double totalMin = 0;
};

// Cost of visiting 'order' using the pairwise cost matrix
static double tourCost(const vector<vector<double>>& d, const vector<int>& order,
                       bool returnToStart) {
    double c = 0;
    for (size_t i = 0; i + 1 < order.size(); i++) c += d[order[i]][order[i + 1]];
    if (returnToStart && order.size() > 1) c += d[order.back()][order[0]];
    return c;
}

// start: where the trip begins. stops: places the user wants to visit.
// Finds a short order to visit all stops (a travelling-salesman style problem,
// solved heuristically: nearest neighbour, then 2-opt).
Itinerary planItinerary(const Graph& g, int start, const vector<int>& stops,
                        Metric m = Metric::DISTANCE, bool returnToStart = false) {
    Itinerary it;

    // nodes[0] = start, nodes[1..] = stops (duplicates removed)
    vector<int> nodes{start};
    for (int s : stops)
        if (find(nodes.begin(), nodes.end(), s) == nodes.end()) nodes.push_back(s);
    int k = (int)nodes.size();

    // Pairwise shortest-path costs between all chosen places (Dijkstra from each)
    vector<vector<double>> d(k, vector<double>(k, INF));
    for (int i = 0; i < k; i++) {
        vector<double> dist;
        vector<int> prev;
        dijkstraAll(g, nodes[i], m, dist, prev);
        for (int j = 0; j < k; j++) d[i][j] = dist[nodes[j]];
    }
    for (int i = 0; i < k; i++)
        for (int j = 0; j < k; j++)
            if (d[i][j] == INF) return it;       // some stop is unreachable

    // Nearest neighbour: always go to the closest unvisited stop
    vector<int> order{0};                        // indices into nodes[]
    vector<bool> used(k, false);
    used[0] = true;
    for (int step = 1; step < k; step++) {
        int cur = order.back(), best = -1;
        for (int j = 0; j < k; j++)
            if (!used[j] && (best == -1 || d[cur][j] < d[cur][best])) best = j;
        used[best] = true;
        order.push_back(best);
    }

    // 2-opt: reverse a segment whenever that shortens the tour (start stays fixed)
    bool improved = true;
    while (improved) {
        improved = false;
        for (int i = 1; i < k - 1; i++) {
            for (int j = i + 1; j < k; j++) {
                vector<int> cand = order;
                reverse(cand.begin() + i, cand.begin() + j + 1);
                if (tourCost(d, cand, returnToStart) + 1e-9 <
                    tourCost(d, order, returnToStart)) {
                    order = cand;
                    improved = true;
                }
            }
        }
    }

    // Convert to place ids and stitch the real road path between consecutive stops
    for (int idx : order) it.order.push_back(nodes[idx]);
    vector<int> legs = it.order;
    if (returnToStart && k > 1) legs.push_back(it.order[0]);

    it.fullPath.push_back(legs[0]);
    for (size_t i = 0; i + 1 < legs.size(); i++) {
        Route leg = dijkstra(g, legs[i], legs[i + 1], m);
        for (size_t p = 1; p < leg.path.size(); p++) it.fullPath.push_back(leg.path[p]);
        it.totalKm += leg.totalKm;
        it.totalMin += leg.totalMin;
    }
    it.found = true;
    return it;
}

// ================================ Helpers ==================================

void printPath(const Graph& g, const vector<int>& path) {
    for (size_t i = 0; i < path.size(); i++)
        cout << g.getPlace(path[i]).name << (i + 1 < path.size() ? " -> " : "");
    cout << "\n";
}

void printRoute(const Graph& g, const string& label, const Route& r) {
    cout << label << ": ";
    if (!r.found) { cout << "no route found\n"; return; }
    printPath(g, r.path);
    cout << "   distance " << r.totalKm << " km, time " << r.totalMin
         << " min, nodes explored: " << r.nodesExplored << "\n";
}

Graph buildSampleGraph() {   // same data as Module 1's demo
    Graph g;
    g.addPlace("India Gate",   "monument", 28.6129, 77.2295);
    g.addPlace("Red Fort",     "monument", 28.6562, 77.2410);
    g.addPlace("Qutub Minar",  "monument", 28.5245, 77.1855);
    g.addPlace("Lotus Temple", "temple",   28.5535, 77.2588);
    g.addPlace("Lodhi Garden", "park",     28.5931, 77.2197);
    g.addPlace("Humayun Tomb", "monument", 28.5933, 77.2507);

    g.addRoad("India Gate", "Red Fort");
    g.addRoad("India Gate", "Lodhi Garden");
    g.addRoad("India Gate", "Humayun Tomb");
    g.addRoad("Lodhi Garden", "Qutub Minar");
    g.addRoad("Humayun Tomb", "Lotus Temple");
    g.addRoad("Lotus Temple", "Qutub Minar");
    g.addRoad("Red Fort", "Humayun Tomb", 9.5);
    return g;
}

// ================================== Demo ===================================

int main() {
    cout << fixed << setprecision(2);
    Graph g = buildSampleGraph();

    int src = g.getId("Red Fort");
    int dst = g.getId("Qutub Minar");

    cout << "=== Shortest route (distance) ===\n";
    printRoute(g, "Dijkstra", dijkstra(g, src, dst));
    printRoute(g, "A*      ", aStar(g, src, dst));

    cout << "\n=== Fastest route (time) ===\n";
    printRoute(g, "Dijkstra", dijkstra(g, src, dst, Metric::TIME));

    cout << "\n=== Itinerary optimization ===\n";
    vector<int> stops = {g.getId("Qutub Minar"), g.getId("Humayun Tomb"),
                         g.getId("Lodhi Garden"), g.getId("Lotus Temple")};
    Itinerary it = planItinerary(g, g.getId("India Gate"), stops);
    if (it.found) {
        cout << "Visit order: ";
        printPath(g, it.order);
        cout << "Full path:   ";
        printPath(g, it.fullPath);
        cout << "Total: " << it.totalKm << " km, " << it.totalMin << " min\n";
    } else {
        cout << "Some stops are unreachable.\n";
    }
    return 0;
}
