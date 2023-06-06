#pragma once

#include <string>
#include <unordered_set>
#include <vector>
#include <utility>

#include "geo.h"

namespace TransportCatalogue{
using namespace geo;

struct Stop{
    std::string name;
    Coordinates coords;
    std::unordered_set<std::string_view> routes;

    bool operator<(Stop other);
    bool operator==(Stop other);
};

struct Bus{
    std::string name;
    std::vector<Stop*> route;
    bool is_round;
    bool operator<(Bus other);
    bool operator==(Bus other);
};

namespace Utility{

    struct StopDistanceHasher{
	    std::size_t operator()(const std::pair<const Stop*, const Stop*>& stops) const;
    };

    struct BusInfo{
        size_t all_count;
        size_t unique_count;
        int length;
        double curvation;
    };
}

} //namespace TransportCatalogue