#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;

// Data types

struct Place {
    int id;
    string name;
    string category;   
    double lat;
    double lon;
};

struct Edge {
    int to;            // destination place id
    double distanceKm; // road distance (weight used by Dijkstra)
    double timeMin;    // estimated travel time in minutes
};

//Graph

class Graph {
private:
    vector<Place> places;                       // index == place id
    vector<vector<Edge>> adj;                   // adjacency list
    unordered_map<string, int> nameToId;        // fast name lookup

    static double toRad(double deg) { return deg * M_PI / 180.0; }

public:
    // Great-circle distance between two coordinates (km)
    static double haversine(double lat1, double lon1, double lat2, double lon2) {
        const double R = 6371.0;
        double dLat = toRad(lat2 - lat1);
        double dLon = toRad(lon2 - lon1);
        double a = sin(dLat / 2) * sin(dLat / 2) +
                   cos(toRad(lat1)) * cos(toRad(lat2)) *
                   sin(dLon / 2) * sin(dLon / 2);
        return R * 2 * atan2(sqrt(a), sqrt(1 - a));
    }

    // Add a place (node). Returns its id.
    int addPlace(const string& name, const string& category, double lat, double lon) {
        if (nameToId.count(name)) return nameToId[name]; // avoid duplicates
        int id = (int)places.size();
        places.push_back({id, name, category, lat, lon});
        adj.emplace_back();
        nameToId[name] = id;
        return id;
    }

    // Add a two-way road. If distanceKm < 0, it is computed from coordinates.
    // avgSpeedKmh is used to estimate travel time.
    void addRoad(int u, int v, double distanceKm = -1, double avgSpeedKmh = 30.0) {
        checkId(u);
        checkId(v);
        if (u == v) return;
        if (distanceKm < 0)
            distanceKm = haversine(places[u].lat, places[u].lon,
                                   places[v].lat, places[v].lon);
        double timeMin = (distanceKm / avgSpeedKmh) * 60.0;
        adj[u].push_back({v, distanceKm, timeMin});
        adj[v].push_back({u, distanceKm, timeMin});
    }

    void addRoad(const string& a, const string& b, double distanceKm = -1) {
        addRoad(getId(a), getId(b), distanceKm);
    }

    //Accessors (used by Dijkstra / A* in Module 2)
    int size() const { return (int)places.size(); }
    const Place& getPlace(int id) const { checkId(id); return places[id]; }
    const vector<Edge>& neighbors(int id) const { checkId(id); return adj[id]; }

    int getId(const string& name) const {
        auto it = nameToId.find(name);
        if (it == nameToId.end()) throw runtime_error("Unknown place: " + name);
        return it->second;
    }

    bool hasPlace(const string& name) const { return nameToId.count(name) > 0; }

    //Loading from CSV files
    // places.csv: name,category,lat,lon
    // roads.csv : placeA,placeB[,distanceKm]
    void loadPlaces(const string& file) {
        ifstream in(file);
        if (!in) throw runtime_error("Cannot open " + file);
        string line;
        getline(in, line); // skip header
        while (getline(in, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string name, cat, lat, lon;
            getline(ss, name, ',');
            getline(ss, cat, ',');
            getline(ss, lat, ',');
            getline(ss, lon, ',');
            addPlace(name, cat, stod(lat), stod(lon));
        }
    }

    void loadRoads(const string& file) {
        ifstream in(file);
        if (!in) throw runtime_error("Cannot open " + file);
        string line;
        getline(in, line); // skip header
        while (getline(in, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string a, b, d;
            getline(ss, a, ',');
            getline(ss, b, ',');
            getline(ss, d, ',');
            addRoad(a, b, d.empty() ? -1 : stod(d));
        }
    }

    //Display
    void print() const {
        cout << fixed << setprecision(2);
        for (const Place& p : places) {
            cout << "[" << p.id << "] " << p.name << " (" << p.category << ")\n";
            for (const Edge& e : adj[p.id])
                cout << "     -> " << places[e.to].name << "  "
                     << e.distanceKm << " km, ~" << e.timeMin << " min\n";
        }
    }

    int edgeCount() const {
        int total = 0;
        for (const auto& list : adj) total += (int)list.size();
        return total / 2; // each road stored twice
    }

private:
    void checkId(int id) const {
        if (id < 0 || id >= (int)places.size())
            throw out_of_range("Invalid place id");
    }
};

//Demo

int main() {
    Graph g;

    // Sample places (name, category, lat, lon)
    g.addPlace("India Gate",    "monument", 28.6129, 77.2295);
    g.addPlace("Red Fort",      "monument", 28.6562, 77.2410);
    g.addPlace("Qutub Minar",   "monument", 28.5245, 77.1855);
    g.addPlace("Lotus Temple",  "temple",   28.5535, 77.2588);
    g.addPlace("Lodhi Garden",  "park",     28.5931, 77.2197);
    g.addPlace("Humayun Tomb",  "monument", 28.5933, 77.2507);

    // Roads (distance computed automatically from coordinates)
    g.addRoad("India Gate", "Red Fort");
    g.addRoad("India Gate", "Lodhi Garden");
    g.addRoad("India Gate", "Humayun Tomb");
    g.addRoad("Lodhi Garden", "Qutub Minar");
    g.addRoad("Humayun Tomb", "Lotus Temple");
    g.addRoad("Lotus Temple", "Qutub Minar");

    // Or give a known road distance manually:
    g.addRoad("Red Fort", "Humayun Tomb", 9.5);

    cout << "Places: " << g.size() << ", Roads: " << g.edgeCount() << "\n\n";
    g.print();

    // To load from files instead:
    //   g.loadPlaces("places.csv");
    //   g.loadRoads("roads.csv");

    return 0;
}
