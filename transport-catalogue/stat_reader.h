#pragma once 

#include <vector>
#include <string>
#include <string_view>
#include <iostream>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <iomanip>

#include "geo.h"
#include "transport_catalogue.h"


class SReader{
public:
    explicit SReader(std::istream& is) : is_(is) {}
    void ReadStat(TransportCatalogue::TransportCatalogue& tc);
    void OutBusStat(TransportCatalogue::TransportCatalogue& tc, std::string_view bus_name);
    void OutStopRoutes(TransportCatalogue::TransportCatalogue& tc, std::string_view stop_name);
private:
    std::istream& is_;

};

