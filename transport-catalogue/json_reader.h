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
#include "transport_router.h"
#include "json_builder.h"

namespace TransportCatalogue{

class JSONReader{
public:
    JSONReader() = default;
    JSONReader(std::istream& is) : 
    is_(is){
        requests_ = json::Load(is_).GetRoot().AsDict();
    }
    void Read(TransportCatalogue& tc, MapRender::MapRenderer& render, Router::TransportRouter& router);
    std::string GetSerializationSettings() const;

private:
    std::istream& is_;
    std::ostream& out_ = std::cout;
    json::Dict requests_;
    Stop ReadStop(const json::Dict& request);
    std::pair<bool, std::vector<std::string>> ReadBus(const json::Dict& request);
    json::Dict OutStopRoutes(const TransportCatalogue& tc, std::string_view stop_name, int id);
    json::Dict OutBusStat(const TransportCatalogue& tc, std::string_view bus_name, int id);
    json::Dict OutMapRender(const TransportCatalogue& tc, MapRender::MapRenderer& render, int id);
    json::Dict OutRouteInfo(const TransportCatalogue& tc, Router::TransportRouter& router,
                                            std::string_view from, std::string_view to, int id);
    svg::Color ProccedColor(const json::Node& request);
};

}//namespace TransportCatalogue

