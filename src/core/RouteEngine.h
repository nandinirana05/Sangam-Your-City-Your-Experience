#pragma once
#include "Graph.h"
#include <vector>
#include <utility>
#include <algorithm>
#include <queue>

class RouteEngine {
private:
    struct Compare {
        bool operator()(const std::pair<double, int>& a, const std::pair<double, int>& b) {
            return a.first > b.first;
        }
    };

    static std::vector<int> buildPath(const std::unordered_map<int, int>& prev, int src, int dst) {
        std::vector<int> path;
        int curr = dst;
        while (curr != src) {
            path.push_back(curr);
            auto it = prev.find(curr);
            if (it == prev.end()) return {}; // disconnected
            curr = it->second;
        }
        path.push_back(src);
        std::reverse(path.begin(), path.end());
        return path;
    }

public:
    static RouteResult dijkstra(const Graph& g, int src, int dst) {
        RouteResult r;
        std::unordered_map<int, double> dist;
        std::unordered_map<int, int> prev;
        std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, Compare> pq;

        for (const auto& p : g.getAllPlaces()) dist[p.first] = INF;
        dist[src] = 0;
        pq.push({0, src});

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            r.nodesExplored++;
            
            if (d > dist[u]) continue;
            if (u == dst) break;

            for (const auto& e : g.neighbors(u)) {
                double alt = dist[u] + e.distanceKm;
                if (alt < dist[e.to]) {
                    dist[e.to] = alt;
                    prev[e.to] = u;
                    pq.push({alt, e.to});
                }
            }
        }

        if (dist[dst] == INF) return r;
        r.found = true;
        r.path = buildPath(prev, src, dst);
        
        // Compute totalKm and Min
        for (size_t i = 0; i < r.path.size() - 1; ++i) {
            int u = r.path[i];
            int v = r.path[i+1];
            for (const auto& e : g.neighbors(u)) {
                if (e.to == v) {
                    r.totalKm += e.distanceKm;
                    r.totalMin += e.timeMin;
                    break;
                }
            }
        }
        return r;
    }

    static Itinerary planItinerary(const Graph& g, int start, const std::vector<int>& stops) {
        Itinerary it;
        std::vector<int> nodes = {start};
        for (int s : stops) {
            if (std::find(nodes.begin(), nodes.end(), s) == nodes.end()) {
                nodes.push_back(s);
            }
        }

        int k = nodes.size();
        std::vector<std::vector<double>> d(k, std::vector<double>(k, INF));
        std::vector<std::vector<RouteResult>> routes(k, std::vector<RouteResult>(k));

        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < k; ++j) {
                if (i == j) d[i][j] = 0;
                else {
                    routes[i][j] = dijkstra(g, nodes[i], nodes[j]);
                    if (routes[i][j].found) d[i][j] = routes[i][j].totalKm;
                }
            }
        }

        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < k; ++j) {
                if (d[i][j] == INF) return it; // Unreachable
            }
        }

        std::vector<int> order = {0};
        std::vector<bool> used(k, false);
        used[0] = true;

        for (int step = 1; step < k; ++step) {
            int cur = order.back();
            int best = -1;
            for (int j = 0; j < k; ++j) {
                if (!used[j] && (best == -1 || d[cur][j] < d[cur][best])) {
                    best = j;
                }
            }
            used[best] = true;
            order.push_back(best);
        }

        // 2-opt missing for brevity, keeping NN for now
        for (int idx : order) it.order.push_back(nodes[idx]);
        
        it.fullPath.push_back(it.order[0]);
        for (size_t i = 0; i < it.order.size() - 1; ++i) {
            int u_idx = order[i];
            int v_idx = order[i+1];
            RouteResult leg = routes[u_idx][v_idx];
            for (size_t p = 1; p < leg.path.size(); ++p) {
                it.fullPath.push_back(leg.path[p]);
            }
            it.totalKm += leg.totalKm;
            it.totalMin += leg.totalMin;
        }

        it.found = true;
        return it;
    }
};
