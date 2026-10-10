#pragma once
#include "Models.h"
#include <unordered_map>
#include <vector>
#include <limits>
#include <algorithm>

const double INF = std::numeric_limits<double>::infinity();

class Graph {
private:
    std::unordered_map<int, Place> places; 
    std::unordered_map<int, std::vector<Edge>> adj;
    
public:
    void addPlace(const Place& p) {
        places[p.id] = p;
        if (adj.find(p.id) == adj.end()) {
            adj[p.id] = std::vector<Edge>();
        }
    }

    void addEdge(int u, int v, double distKm, double timeMin) {
        adj[u].push_back({v, distKm, timeMin});
    }

    const Place* getPlace(int id) const {
        auto it = places.find(id);
        if (it != places.end()) return &it->second;
        return nullptr;
    }

    const std::vector<Edge>& neighbors(int id) const {
        auto it = adj.find(id);
        if (it != adj.end()) return it->second;
        static std::vector<Edge> empty;
        return empty;
    }

    const std::unordered_map<int, Place>& getAllPlaces() const { return places; }
};
