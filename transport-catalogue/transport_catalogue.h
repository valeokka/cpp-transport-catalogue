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
#include <optional>
#include <memory>

#include "geo.h"
#include "domain.h"
#include "graph.h"
#include "router.h"

namespace TransportCatalogue{

class TransportCatalogue{
public:

    TransportCatalogue() {}
    void AddStop(std::string name_in, geo::Coordinates coord_in);
    void AddBus(std::string name_in, bool is_round, const std::vector<std::string>& stops_in);
    const Stop* GetStop(std::string_view stop) const;    
    const Bus* GetBus(std::string_view bus) const;
    std::optional <Utility::BusInfo> GetBusInfo(std::string_view bus) const;
    std::optional <std::unordered_set<std::string_view>> GetStopRoutes(std::string_view stop) const;
    void SetDistance(std::string_view left, std::string_view right, int distance);
    size_t GetStopCount() const;    
    size_t GetBusCount() const;
    const std::unordered_map<std::string_view, Stop*> RequestStops() const;
    const std::unordered_map<std::string_view, Bus*> RequestBuses() const;
    int GetDistance(const Stop* lhs, const Stop* rhs) const;
	size_t GetVertexCount() const;

private:
    std::deque<Stop> stop_list_;
    std::deque<Bus> bus_list_;
    size_t vertex_count_ = 0;
    std::unordered_map<std::string_view, Stop*> stops_by_name_;
    std::unordered_map<std::string_view, Bus*> bus_by_name_;
    std::unordered_map<std::pair<Stop*, Stop*>, int, Utility::StopDistanceHasher> distances_;

};
}