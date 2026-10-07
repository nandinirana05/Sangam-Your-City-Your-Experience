#include <iostream>
#include <vector>
#include <string>
#include <sqlite3.h>
using namespace std;

//STRUCTURE FOR PLACE
struct Place {
    int id;
    int category_id;
    string name;
    string description;
    double latitude;
    double longitude;
};

//GLOBAL VECTOR TO STORE PLACES
vector<Place> allPlaces;

//LOAD PLACES FROM DATABASE
bool loadPlacesFromDB(const string& dbPath) {
    sqlite3* db;

    int rc = sqlite3_open(dbPath.c_str(), &db);
    /*rc stores the return code of the sqlite3_open function. 
    If rc is not SQLITE_OK, it means there was an error opening the database, 
    and we print an error message and return false to indicate failure.*/

    if (rc) {
        cout << "Error opening database: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    string sql = "SELECT id, category_id, name, description, latitude, longitude FROM places;";
    sqlite3_stmt* stmt;

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cout << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return false;
    }

    allPlaces.clear();  // clear old data

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Place p;
        p.id          = sqlite3_column_int(stmt, 0);
        p.category_id = sqlite3_column_int(stmt, 1);
        p.name        = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        
        const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        p.description = desc ? desc : "";

        p.latitude    = sqlite3_column_double(stmt, 4);
        p.longitude   = sqlite3_column_double(stmt, 5);

        allPlaces.push_back(p);   // Store in Vector
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    cout << "Successfully loaded " << allPlaces.size() << " places into Vector." << endl;
    return true;
}

//DISPLAY ALL PLACES
void displayAllPlaces() {
    if (allPlaces.empty()) {
        cout << "No places loaded." << endl;
        return;
    }

    cout << "\nALL PLACES (Vector)\n";
    for (const auto& p : allPlaces) {
        cout << "ID: " << p.id 
             << " | Name: " << p.name 
             << " | Category ID: " << p.category_id
             << " | Lat: " << p.latitude 
             << " | Lng: " << p.longitude << endl;
    }
    cout << "\n";
}

//SEARCH PLACE BY NAME
void searchPlaceByName(const string& keyword) {
    bool found = false;
    cout << "\nSearching for: \"" << keyword << "\"\n";

    for (const auto& p : allPlaces) {
        if (p.name.find(keyword) != string::npos) {
            cout << "→ Found: " << p.name 
                 << " (ID: " << p.id << ")" << endl;
            found = true;
        }
    }

    if (!found) {
        cout << "No place found with that name." << endl;
    }
}

//FILTER BY CATEGORY
void filterByCategory(int categoryId) {
    cout << "\nPlaces in Category ID " << categoryId << ":\n";
    bool found = false;

    for (const auto& p : allPlaces) {
        if (p.category_id == categoryId) {
            cout << "→ " << p.name << endl;
            found = true;
        }
    }

    if (!found) {
        cout << "No places found in this category." << endl;
    }
}

//MAIN FUNCTION
int main() {
    string dbPath = "sangam.db";   // Make sure this file is in the same folder

    //Load places from SQLite into Vector
    if (!loadPlacesFromDB(dbPath)) {
        return 1;
    }

    //Display all places
    displayAllPlaces();

    //Search example
    searchPlaceByName("Park");

    //Filter by category example (3 = Park)
    filterByCategory(3);

    return 0;
}
