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


class IReader{
public:
    explicit IReader(std::istream& is) : is_(is) {}
    void ReadInput(TransportCatalogue::TransportCatalogue& tc);
private:
    std::istream& is_;
};

 