#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <transport_catalogue.pb.h>

void Serialize(const TransportCatalogue::TransportCatalogue& tc,
	const MapRender::MapRenderer& renderer,
	const TransportCatalogue::Router::TransportRouter& router,
	std::ostream& output);

serialize::Distance Serialize(std::string first, std::string second, int distance);

serialize::Stop Serialize(const TransportCatalogue::Stop* stop);

serialize::Bus Serialize(const TransportCatalogue::Bus* bus);

serialize::RenderSettings GetRenderSettingSerialize(const MapRender::RenderOptions& render_settings);

// serialize::RouterSettings GetRouterSettingSerialize(const json::Node& router_settings);

serialize::Router Serialize(const TransportCatalogue::Router::TransportRouter& router);

void Deserialize(
	TransportCatalogue::TransportCatalogue& tc
	, MapRender::MapRenderer& render
	, TransportCatalogue::Router::TransportRouter& router
	, std::istream& input);