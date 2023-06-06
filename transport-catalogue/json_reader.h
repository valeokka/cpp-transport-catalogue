#pragma once

#include <iostream>
#include <utility>
#include <vector>
#include <vector>
#include <string_view>
#include <unordered_map>
#include <set>

#include "json.h"
#include "svg.h"
#include "map_renderer.h"
#include "transport_catalogue.h"

namespace TransportCatalogue{

class JSONReader{
public:
    JSONReader() = default;
    JSONReader(std::istream& is);
    void Read(TransportCatalogue& tc, MapRender::MapRenderer& render);

private:
    std::istream& is_;
    std::ostream& out_ = std::cout;
    Stop ReadStop(const json::Dict& request);
    std::pair<bool, std::vector<std::string>> ReadBus(const json::Dict& request);
    json::Dict OutStopRoutes(const TransportCatalogue& tc, std::string_view stop_name, int id);
    json::Dict OutBusStat(const TransportCatalogue& tc, std::string_view bus_name, int id);
    json::Dict OutMapRender(const TransportCatalogue& tc, MapRender::MapRenderer& render, int id);
    svg::Color ProccedColor(const json::Node& request);
};

}//namespace TransportCatalogue

