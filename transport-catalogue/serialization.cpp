#include "serialization.h"



using namespace std;

void Serialize(const TransportCatalogue::TransportCatalogue& tc,
	const MapRender::MapRenderer& renderer,
	const TransportCatalogue::Router::TransportRouter& router,
	std::ostream& output) {
	serialize::TransportCatalogue database;
	for (const auto& [name, stop] : tc.RequestStops()) {
		*database.add_stop() = Serialize(stop);
	}
	for (const auto& [name, bus] : tc.RequestBuses()) {
		*database.add_bus() = Serialize(bus);
	}
    for (const auto& [stops, distance] : tc.GetDistances()) {
		std::string first = stops.first->name;
		std::string second = stops.second->name;
		int dist = distance;
		*database.add_distance() = Serialize(first, second, dist);
	}
	*database.mutable_render_settings() = GetRenderSettingSerialize(renderer.GetRenderSettings());
	*database.mutable_router() = Serialize(router);
	database.SerializeToOstream(&output);
}

serialize::Stop Serialize(const TransportCatalogue::Stop* stop) {
	serialize::Stop result;
	result.set_name(stop->name);
	result.add_coordinate(stop->coords.lat);
	result.add_coordinate(stop->coords.lng);

	return result;
}

serialize::Bus Serialize(const TransportCatalogue::Bus* bus) {
	serialize::Bus result;
	result.set_name(bus->name);
	for (const auto& stop : bus->route) {
		result.add_stop(stop->name);
	}
	result.set_is_circle(bus->is_round);

	return result;
}

serialize::Distance Serialize(std::string first, std::string second, int distance) {
	serialize::Distance result;
	
	result.set_stop_from(first);
	result.set_stop_to(second);
	result.set_distance(distance);

	return result;
}

serialize::Point GetPointSerialize(const pair<double, double>& point) {
	serialize::Point result;
	result.set_x(point.first);
	result.set_y(point.second);
	return result;
}

serialize::Color GetColorSerialize(const svg::Color& color) {
	serialize::Color result;

		if (std::holds_alternative<svg::Rgb>(color)) {
			svg::Rgb rgb = std::get<svg::Rgb>(color);
			serialize::RGB rgb_p;
			rgb_p.set_red(rgb.red);
			rgb_p.set_green(rgb.green);
			rgb_p.set_blue(rgb.blue);
			*result.mutable_rgb() = rgb_p;
		} else if (std::holds_alternative<svg::Rgba>(color)) {
			svg::Rgba rgba = std::get<svg::Rgba>(color);
			serialize::RGBA rgba_p;
			rgba_p.set_red(rgba.red);
			rgba_p.set_green(rgba.green);
			rgba_p.set_blue(rgba.blue);
			rgba_p.set_opacity(rgba.opacity);
			*result.mutable_rgba() = rgba_p;
		}else if(std::holds_alternative<std::string>(color)){
			std::string color_string = std::get<std::string>(color);
			result.set_name(color_string);
		}
	return result;
}

serialize::RenderSettings GetRenderSettingSerialize(const MapRender::RenderOptions& render_settings) {
	serialize::RenderSettings result;
	result.set_width(render_settings.width);
	result.set_height(render_settings.height);
	result.set_padding(render_settings.padding);
	result.set_stop_radius(render_settings.stop_radius);
	result.set_line_width(render_settings.line_width);
	result.set_bus_label_font_size(render_settings.bus_label_font_size);
	*result.mutable_bus_label_offset() = GetPointSerialize( {render_settings.bus_label_offset_1,render_settings.bus_label_offset_2} );
	result.set_stop_label_font_size(render_settings.stop_label_font_size);
	*result.mutable_stop_label_offset() = GetPointSerialize({render_settings.stop_label_offset_1, render_settings.stop_label_offset_2});
	*result.mutable_underlayer_color() = GetColorSerialize(render_settings.underlayer_color);
	result.set_underlayer_width(render_settings.underlayer_width);
	for (const auto& c : render_settings.color_palette) {
		*result.add_color_palette() = GetColorSerialize(c);
	}
	return result;
}

serialize::RouterSettings GetRouterSettingSerialize(const pair<int, double> router_settings) {
	serialize::RouterSettings result;
	result.set_bus_wait_time(router_settings.first);
	result.set_bus_velocity(router_settings.second);
	return result;
}

serialize::Graph GetGraphSerialize(const graph::DirectedWeightedGraph<double>& graph) {
	serialize::Graph result;
	size_t vertex_count = graph.GetVertexCount();
	size_t edge_count = graph.GetEdgeCount();
	for (size_t i = 0; i < edge_count; ++i) {
		const graph::Edge<double>& edge = graph.GetEdge(i);
		serialize::Edge s_edge;
		s_edge.set_from(edge.from);
		s_edge.set_to(edge.to);
		s_edge.set_weight(edge.weight);
		*result.add_edge() = s_edge;
	}
	for (size_t i = 0; i < vertex_count; ++i) {
		serialize::Vertex vertex;
		for (const auto& edge_id : graph.GetIncidentEdges(i)) {
			vertex.add_edge_id(edge_id);
		}
		*result.add_vertex() = vertex;
	}
	return result;
}

serialize::Router Serialize(const TransportCatalogue::Router::TransportRouter& router) {
	serialize::Router result;
	*result.mutable_router_settings() = GetRouterSettingSerialize(router.GetRouterSettings());
	*result.mutable_graph() = GetGraphSerialize(router.GetGraph());
	for (const auto& [name, id] : router.GetStopIds()) {
		serialize::StopId stop_id;
		stop_id.set_name(name);
		stop_id.set_id(id);
		*result.add_stop_id() = stop_id;
	}
	return result;
}

void AddBusFromDB(TransportCatalogue::TransportCatalogue& tc, const serialize::TransportCatalogue& database) {
	for (size_t i = 0; i < database.bus_size(); ++i) {
		const serialize::Bus& bus = database.bus(i);
		std::vector<std::string> stops(bus.stop_size());

		for (size_t j = 0; j < stops.size(); ++j) {
			stops[j] = tc.GetStop(bus.stop(j))->name;
		}
		tc.AddBus(bus.name(), bus.is_circle(), stops);
		
	}
}

void AddStopFromDB(TransportCatalogue::TransportCatalogue& tc, const serialize::TransportCatalogue& database) {
	for (size_t i = 0; i < database.stop_size(); ++i) {
		const serialize::Stop& stop = database.stop(i);
		tc.AddStop(stop.name(), { stop.coordinate(0), stop.coordinate(1) });
	}
}

void SetDistancesFromDB(TransportCatalogue::TransportCatalogue& tc, const serialize::TransportCatalogue& database) {
	for (size_t i = 0; i < database.distance_size(); ++i) {
		const serialize::Distance& distance = database.distance(i);
		tc.SetDistance(distance.stop_from(), distance.stop_to(), distance.distance() ) ;
	}
}

svg::Color GetColorFromDB(serialize::Color color){
	svg::Color result;
	if(color.has_rgb()){
		svg::Rgb rgb;
		rgb.red = color.rgb().red();
		rgb.blue = color.rgb().blue();
		rgb.green = color.rgb().green();
		result = rgb;
	} else if(color.has_rgba()){
		svg::Rgba rgba;
		rgba.red = color.rgba().red();
		rgba.blue = color.rgba().blue();
		rgba.green = color.rgba().green();
		rgba.opacity = color.rgba().opacity();
		result = rgba;
	}else{
		result = color.name();
	}
	return result;

}

MapRender::RenderOptions GetRenderSettingsFromDB(const serialize::TransportCatalogue& database) {
	const serialize::RenderSettings& rs = database.render_settings();
	MapRender::RenderOptions result;

	result.width = rs.width();
	result.height = rs.height();
	result.padding = rs.padding();
	result.stop_radius = rs.stop_radius();
	result.line_width = rs.line_width();
	result.bus_label_font_size = rs.bus_label_font_size();
	result.bus_label_offset_1 = rs.bus_label_offset().x();
	result.bus_label_offset_2 = rs.bus_label_offset().y();
	result.stop_label_font_size = rs.stop_label_font_size();
	result.stop_label_offset_1 = rs.stop_label_offset().x();
	result.stop_label_offset_2 = rs.stop_label_offset().y();
	result.underlayer_color = GetColorFromDB(rs.underlayer_color());
	result.underlayer_width = rs.underlayer_width();
	
	for(int i = 0; i < rs.color_palette().size(); ++i){
		result.color_palette.push_back(GetColorFromDB(rs.color_palette(i)));
	}

	return result;
}



std::pair<int, double> GetRouterSettingsFromDB(const serialize::Router& router) {
	const serialize::RouterSettings& rs = router.router_settings();
	return {rs.bus_wait_time(), rs.bus_velocity()};
}

graph::DirectedWeightedGraph<double> GetGraphFromDB(const serialize::Router& router) {
	graph::DirectedWeightedGraph<double> result;
	const serialize::Graph& graph_s = router.graph();
	std::vector<graph::Edge<double>> edges(graph_s.edge_size());
	std::vector<std::vector<graph::EdgeId>> incidence_lists(graph_s.vertex_size());

	for (size_t i = 0; i < edges.size(); ++i) {
		const serialize::Edge& edge_s = graph_s.edge(i);
		edges[i] = { static_cast<size_t>(edge_s.from()), static_cast<size_t>(edge_s.to()), edge_s.weight() };
	}
	result.SetEdges(edges);

	for (size_t i = 0; i < incidence_lists.size(); ++i) {
		const serialize::Vertex& vertex_s = graph_s.vertex(i);
		incidence_lists[i].reserve(vertex_s.edge_id_size());
		for (const auto& id : vertex_s.edge_id()) {
			incidence_lists[i].push_back(id);
		}
	}
	result.SetIncedenceLists(incidence_lists);

	return result;
}

std::unordered_map<std::string, graph::VertexId> GetStopIdsFromDB(const serialize::Router& router) {
	std::unordered_map<std::string, graph::VertexId> result;
	for (const auto& stop : router.stop_id()) {
		result[stop.name()] = stop.id();
	}
	return result;
}


void Deserialize(
	TransportCatalogue::TransportCatalogue& tc
	, MapRender::MapRenderer& render
	, TransportCatalogue::Router::TransportRouter& router
	, std::istream& input){


	serialize::TransportCatalogue database;
	database.ParseFromIstream(&input);

	AddStopFromDB(tc, database);
	AddBusFromDB(tc, database);
	SetDistancesFromDB(tc, database);

	render.SetSettings(GetRenderSettingsFromDB(database));
	
	auto router_s = database.router();
	auto router_settings = GetRouterSettingsFromDB(router_s);
	router.SetBusWaitTime(router_settings.first);
	router.SetBusSpeed(router_settings.second);
	router.SetStopIds(GetStopIdsFromDB(router_s));
	router.SetGraph(GetGraphFromDB(router_s));

}