#include "transport_catalogue.h"
#include "request_handler.h"
#include "json_reader.h"
#include "json.h"
#include "geo.h"
#include "map_renderer.h"
#include "transport_router.h"
#include "serialization.h"

#include <transport_catalogue.pb.h>

#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

using namespace std::literals;
using namespace TransportCatalogue;
void PrintUsage(std::ostream& stream = std::cerr) {
	stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		PrintUsage();
		return 1;
	}

	const std::string_view mode(argv[1]);

	if (mode == "make_base"sv) {
		JSONReader js(std::cin);
		TransportCatalogue::TransportCatalogue tc;
        MapRender::MapRenderer render;
        Router::TransportRouter router;
		js.Read(tc, render, router);

		std::ofstream ofs(js.GetSerializationSettings(), std::ios::binary);
				
		serialization::Serialize(tc, render, router, ofs);
		
	}
	else if (mode == "process_requests"sv) {
		JSONReader js(std::cin);
		std::ifstream file(js.GetSerializationSettings(), std::ios::binary);
		if (file) {

			TransportCatalogue::TransportCatalogue tc;
			MapRender::MapRenderer render;
			TransportCatalogue::Router::TransportRouter router;

			serialization::Deserialize(tc, render, router, file);
            js.Read(tc, render, router);
		}
	}
	else {
		PrintUsage();
		return 1;
	}
}