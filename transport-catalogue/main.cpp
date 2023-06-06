#include <iostream>
#include <fstream>
#include "transport_catalogue.h"
#include "json_reader.h"

using namespace TransportCatalogue;

int main() {

    JSONReader js(std::cin);
    MapRender::MapRenderer render;
    TransportCatalogue::TransportCatalogue tc;
    js.Read(tc,render);
}