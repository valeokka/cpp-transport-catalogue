#include <iostream>
#include <fstream>
#include "transport_catalogue.h"
#include "json_reader.h"

using namespace TransportCatalogue;

int main() {

    JSONReader js(std::cin);
    TransportCatalogue::TransportCatalogue tc;
    MapRender::MapRenderer render;
    TransportCatalogue::Router::TransportRouter router;
    js.Read(tc,render,router);
}