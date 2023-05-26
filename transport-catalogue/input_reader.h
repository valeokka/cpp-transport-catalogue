#pragma once 

#include <vector>
#include <string>
#include <string_view>
#include <iostream>
#include <utility>
#include <unordered_map>
#include <algorithm>
#include <tuple>

#include "geo.h"
#include "transport_catalogue.h"

namespace TransportCatalogue{
class IReader{
public:
    explicit IReader(std::istream& is) : is_(is) {}
    void ReadInput(TransportCatalogue& tc);
private:
    std::istream& is_;
    void ReadBus(std::string_view text_sv, std::unordered_map<std::string, Stop>& stops, 
std::unordered_map<std::string, std::pair<std::string, std::vector<std::string>>>& buses);
    void ReadStop(std::string_view text_sv, std::unordered_map<std::string, Stop>& stops, 
                std::vector<std::tuple<std::string, std::string, int>>& distance_stops);
};
}
 