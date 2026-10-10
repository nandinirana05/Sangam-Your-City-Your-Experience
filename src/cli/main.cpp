#include "../../webpage/json.hpp"
#include "../core/Models.h"
#include "../core/Database.h"
#include "../core/Graph.h"
#include "../core/RouteEngine.h"
#include "../core/Trie.h"
#include <iostream>
#include <string>

using json = nlohmann::json;

void to_json(json& j, const Place& p) {
    j = json{{"id", p.id}, {"category_id", p.category_id}, {"name", p.name}, {"category", p.category}, {"description", p.description}, {"lat", p.lat}, {"lng", p.lon}};
}
void to_json(json& j, const Category& c) {
    j = json{{"id", c.id}, {"name", c.name}};
}
void to_json(json& j, const Itinerary& it) {
    j = json{{"id", it.id}, {"name", it.name}, {"order", it.order}, {"fullPath", it.fullPath}, {"totalKm", it.totalKm}, {"totalMin", it.totalMin}};
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Expected JSON command argument\n";
        return 1;
    }

    std::string input = argv[1];
    json req = json::parse(input);
    std::string action = req["action"];

    Database db("SQlite_Database/sangam.db");
    if (!db.isOpen()) {
        std::cout << R"({"error": "DB_FAILED"})" << "\n";
        return 1;
    }

    if (action == "health") {
        std::cout << R"({"status": "ready"})" << "\n";
        return 0;
    }

    if (action == "get_places") {
        std::vector<Place> places = db.loadPlaces();
        json j = places;
        std::cout << j.dump() << "\n";
        return 0;
    }

    if (action == "get_categories") {
        std::vector<Category> cats = db.loadCategories();
        json j = cats;
        std::cout << j.dump() << "\n";
        return 0;
    }

    if (action == "search") {
        std::string q = req["q"];
        std::vector<Place> places = db.loadPlaces();
        Trie trie;
        std::unordered_map<int, Place> placeMap;
        for (const auto& p : places) {
            trie.insert(p.name, p.id);
            placeMap[p.id] = p;
        }
        std::vector<int> ids = trie.searchPrefix(q);
        std::vector<Place> results;
        for (int id : ids) {
            if (placeMap.count(id)) results.push_back(placeMap[id]);
        }
        json j = results;
        std::cout << j.dump() << "\n";
        return 0;
    }

    if (action == "get_itineraries") {
        std::vector<Itinerary> trips = db.loadItineraries();
        json j = trips;
        std::cout << j.dump() << "\n";
        return 0;
    }

    if (action == "save_itinerary") {
        std::string name = req["name"];
        std::vector<int> stops = req["stops"];
        int id = db.saveItinerary(name, stops);
        if (id != -1) {
            std::cout << json{{"id", id}}.dump() << "\n";
        } else {
            std::cout << R"({"error": "FAILED"})" << "\n";
        }
        return 0;
    }

    if (action == "optimize_route") {
        std::vector<int> stops = req["stops"];
        std::vector<Place> places = db.loadPlaces();
        std::vector<Database::ConnectionRecord> connections = db.loadConnections();
        Graph g;
        for (const auto& p : places) g.addPlace(p);
        for (const auto& c : connections) g.addEdge(c.from_id, c.to_id, c.distance_km, c.travel_time_min);
        
        Itinerary it = RouteEngine::planItinerary(g, stops[0], stops);
        if (it.found) {
            std::cout << json(it).dump() << "\n";
        } else {
            std::cout << R"({"error": "Route not found"})" << "\n";
        }
        return 0;
    }

    std::cout << R"({"error": "UNKNOWN_ACTION"})" << "\n";
    return 1;
}
