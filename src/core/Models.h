#pragma once
#include <string>
#include <vector>
#include <cmath>

struct Place {
    int id;
    int category_id;
    std::string name;
    std::string category;
    std::string description;
    double lat;
    double lon;
};

struct Edge {
    int to;
    double distanceKm;
    double timeMin;
};

struct RouteResult {
    bool found = false;
    std::vector<int> path; // place ids
    double totalKm = 0;
    double totalMin = 0;
    int nodesExplored = 0;
};

struct Itinerary {
    int id = 0;
    std::string name;
    bool found = false;
    std::vector<int> order;    // stops in visiting order
    std::vector<int> fullPath; // complete path passing through graph nodes
    double totalKm = 0;
    double totalMin = 0;
};

struct Category {
    int id;
    std::string name;
};
