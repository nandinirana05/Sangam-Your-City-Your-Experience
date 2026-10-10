#include "../../webpage/httplib.h"
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

void set_cors(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

int main() {
    std::cout << "Loading Database...\n";
    Database db("SQlite_Database/sangam.db");
    
    if (!db.isOpen()) {
        std::cerr << "Could not open SQLite database.\n";
        return 1;
    }

    std::vector<Category> categories = db.loadCategories();
    std::vector<Place> places = db.loadPlaces();
    std::vector<Database::ConnectionRecord> connections = db.loadConnections();

    Graph g;
    Trie trie;
    std::unordered_map<int, Place> placeMap;

    for (const auto& p : places) {
        g.addPlace(p);
        trie.insert(p.name, p.id);
        placeMap[p.id] = p;
    }

    for (const auto& c : connections) {
        g.addEdge(c.from_id, c.to_id, c.distance_km, c.travel_time_min);
    }

    httplib::Server svr;

    svr.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) {
        set_cors(res);
    });

    svr.Get("/api/health", [](const httplib::Request&, httplib::Response& res) {
        set_cors(res);
        res.set_content(R"({"status": "ready"})", "application/json");
    });

    svr.Get("/api/places", [&](const httplib::Request&, httplib::Response& res) {
        set_cors(res);
        json j = places;
        res.set_content(j.dump(), "application/json");
    });
    
    // Fallback for frontend calling /places directly (as in index.html)
    svr.Get("/places", [&](const httplib::Request&, httplib::Response& res) {
        set_cors(res);
        json j = places;
        res.set_content(j.dump(), "application/json");
    });

    svr.Get("/api/categories", [&](const httplib::Request&, httplib::Response& res) {
        set_cors(res);
        json j = categories;
        res.set_content(j.dump(), "application/json");
    });

    auto searchHandler = [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        if (req.has_param("q")) {
            std::string q = req.get_param_value("q");
            std::vector<int> ids = trie.searchPrefix(q);
            std::vector<Place> results;
            for (int id : ids) {
                if (placeMap.count(id)) {
                    results.push_back(placeMap[id]);
                }
            }
            json j = results;
            res.set_content(j.dump(), "application/json");
        } else {
            res.set_content("[]", "application/json");
        }
    };
    svr.Get("/api/search", searchHandler);
    svr.Get("/search", searchHandler); // fallback for frontend

    svr.Post("/api/route/optimize", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            auto body = json::parse(req.body);
            std::vector<int> stops = body["stops"];
            if (stops.empty()) {
                res.status = 400;
                res.set_content(R"({"error": "Empty stops"})", "application/json");
                return;
            }
            Itinerary it = RouteEngine::planItinerary(g, stops[0], stops);
            if (it.found) {
                json j = it;
                res.set_content(j.dump(), "application/json");
            } else {
                res.status = 404;
                res.set_content(R"({"error": "Route not found"})", "application/json");
            }
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(std::string(R"({"error": ")") + e.what() + R"("})", "application/json");
        }
    });

    svr.Get("/api/itineraries", [&](const httplib::Request&, httplib::Response& res) {
        set_cors(res);
        std::vector<Itinerary> trips = db.loadItineraries();
        json j = trips;
        res.set_content(j.dump(), "application/json");
    });

    svr.Post("/api/itineraries", [&](const httplib::Request& req, httplib::Response& res) {
        set_cors(res);
        try {
            auto body = json::parse(req.body);
            std::string name = body["name"];
            std::vector<int> stops = body["stops"];
            int id = db.saveItinerary(name, stops);
            if (id != -1) {
                json j = {{"id", id}};
                res.set_content(j.dump(), "application/json");
            } else {
                res.status = 500;
                res.set_content(R"({"error": "Failed to save"})", "application/json");
            }
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(R"({"error": "Bad request"})", "application/json");
        }
    });

    std::cout << "Server starting on http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
    return 0;
}
