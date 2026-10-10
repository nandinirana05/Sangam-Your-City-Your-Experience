#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "Models.h"

class Database {
private:
    sqlite3* db;

public:
    Database(const std::string& path) {
        if (sqlite3_open(path.c_str(), &db)) {
            std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
            db = nullptr;
        }
    }

    ~Database() {
        if (db) sqlite3_close(db);
    }

    bool isOpen() const { return db != nullptr; }

    std::vector<Category> loadCategories() {
        std::vector<Category> categories;
        if (!isOpen()) return categories;
        
        std::string sql = "SELECT id, name FROM categories;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                categories.push_back({
                    sqlite3_column_int(stmt, 0),
                    reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))
                });
            }
            sqlite3_finalize(stmt);
        }
        return categories;
    }

    std::vector<Place> loadPlaces() {
        std::vector<Place> places;
        if (!isOpen()) return places;

        std::string sql = "SELECT p.id, p.category_id, p.name, p.description, p.latitude, p.longitude, c.name FROM places p LEFT JOIN categories c ON p.category_id = c.id;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                Place p;
                p.id = sqlite3_column_int(stmt, 0);
                p.category_id = sqlite3_column_int(stmt, 1);
                p.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                const char* desc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
                p.description = desc ? desc : "";
                p.lat = sqlite3_column_double(stmt, 4);
                p.lon = sqlite3_column_double(stmt, 5);
                const char* cat = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
                p.category = cat ? cat : "";
                places.push_back(p);
            }
            sqlite3_finalize(stmt);
        }
        return places;
    }

    struct ConnectionRecord {
        int from_id;
        int to_id;
        double distance_km;
        double travel_time_min;
    };

    std::vector<ConnectionRecord> loadConnections() {
        std::vector<ConnectionRecord> conns;
        if (!isOpen()) return conns;

        std::string sql = "SELECT from_place_id, to_place_id, distance_km, travel_time_min FROM connections;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                conns.push_back({
                    sqlite3_column_int(stmt, 0),
                    sqlite3_column_int(stmt, 1),
                    sqlite3_column_double(stmt, 2),
                    sqlite3_column_double(stmt, 3)
                });
            }
            sqlite3_finalize(stmt);
        }
        return conns;
    }

    int saveItinerary(const std::string& name, const std::vector<int>& place_ids) {
        if (!isOpen()) return -1;
        
        sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
        
        std::string sql = "INSERT INTO itineraries (user_id, name) VALUES (1, ?);";
        sqlite3_stmt* stmt;
        int itinerary_id = -1;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
            if (sqlite3_step(stmt) == SQLITE_DONE) {
                itinerary_id = sqlite3_last_insert_rowid(db);
            }
            sqlite3_finalize(stmt);
        }

        if (itinerary_id != -1) {
            sql = "INSERT INTO itinerary_places (itinerary_id, place_id, visit_order) VALUES (?, ?, ?);";
            if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
                for (size_t i = 0; i < place_ids.size(); ++i) {
                    sqlite3_bind_int(stmt, 1, itinerary_id);
                    sqlite3_bind_int(stmt, 2, place_ids[i]);
                    sqlite3_bind_int(stmt, 3, i);
                    sqlite3_step(stmt);
                    sqlite3_reset(stmt);
                }
                sqlite3_finalize(stmt);
            }
            sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
        } else {
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        }
        return itinerary_id;
    }
    
    std::vector<Itinerary> loadItineraries() {
        std::vector<Itinerary> trips;
        if (!isOpen()) return trips;

        std::string sql = "SELECT id, name FROM itineraries;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                Itinerary it;
                it.id = sqlite3_column_int(stmt, 0);
                it.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                trips.push_back(it);
            }
            sqlite3_finalize(stmt);
        }

        for (auto& it : trips) {
            std::string sql2 = "SELECT place_id FROM itinerary_places WHERE itinerary_id = ? ORDER BY visit_order ASC;";
            sqlite3_stmt* stmt2;
            if (sqlite3_prepare_v2(db, sql2.c_str(), -1, &stmt2, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(stmt2, 1, it.id);
                while (sqlite3_step(stmt2) == SQLITE_ROW) {
                    it.order.push_back(sqlite3_column_int(stmt2, 0));
                }
                sqlite3_finalize(stmt2);
            }
        }
        return trips;
    }
};
