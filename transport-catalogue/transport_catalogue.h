#pragma once

#include <deque>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <string_view>
#include <utility>
#include <iomanip>
#include <iostream>

#include "geo.h"

namespace TransportCatalogue{
    
struct Stop{
    std::string name;
    Coordinates coords;
    std::unordered_set<std::string_view> routes;

    bool operator<(Stop other){
        return name < other.name;
    }

    bool operator==(Stop other){
        return name == other.name;
    }
};

struct Bus{
    std::string name;
    std::vector<Stop*> route;

    bool operator<(Bus other){
        return name < other.name;
    }

    bool operator==(Bus other){
        return name == other.name;
    }
};

namespace Utility{

    struct StopDistanceHasher{
	    std::size_t operator()(const std::pair<const  Stop*,   const  Stop*>& stops) const {
	        int hash_one = stops.first->coords.lat * 37 * 37 * 37 + stops.first->coords.lng * 37 * 37 + stops.first->routes.size() * 37;
            int hash_two = stops.second->coords.lat * 12 * 12 * stops.second->coords.lng * 12 * 12;
            return hash_one * 1000 + hash_two;
	    }
    };

    struct BusInfo{
        size_t all_count;
        size_t unique_count;
        int length;
        double curvation;
    };
}

class TransportCatalogue{
public:

    TransportCatalogue() {}
    void AddStop(std::string name_in, Coordinates coord_in);
    void AddStop(std::string name_in, double lat_in, double lng_in);
    void AddBus(std::string name_in, std::vector<std::string> stops_in);
    const Stop* GetStop(std::string_view stop) const;    
    const Bus* GetBus(std::string_view bus) const;
    Utility::BusInfo GetBusInfo(std::string_view bus);
    std::unordered_set<std::string_view> GetStopRoutes(std::string_view stop);
    void AddDistance(std::string left, std::string right, int distance);
    size_t GetStopCount() const;    
    size_t GetBusCount() const;

private:
    
    std::deque<Stop> stop_list_;
    std::deque<Bus> bus_list_;
    std::unordered_map<std::string_view, Stop*> stops_by_name_;
    std::unordered_map<std::string_view, Bus*> bus_by_name_;
    std::unordered_map<std::pair<Stop*, Stop*>, int, Utility::StopDistanceHasher> distances_;

};
}