#include "transport_router.h"

namespace TransportCatalogue{
namespace Router {

void TransportRouter::SetBusWaitTime(int time){bus_wait_time_ = time;}
void TransportRouter::SetBusSpeed(double speed){bus_speed_ = speed;}

std::optional<CompletedRoute> TransportRouter::ResultRoute(const TransportCatalogue& tc, 
											  graph::VertexId from, graph::VertexId to){

	const std::unordered_map<std::string_view, Bus*>& info_bus = tc.RequestBuses();
	if (graph_.GetVertexCount() == 0){CreateGraph(info_bus, tc);}
	return ComputeRoute(from, to);
}

void TransportRouter::CreateGraph(const std::unordered_map <std::string_view, Bus*>& info_bus, 
                                                                const TransportCatalogue& tc) {
	graph_.SetVertexCount(tc.GetVertexCount());
	double bus_velocity = bus_speed_ * M_MMIN;

	for (auto [bus_name, info] : info_bus) {
		const Bus* bus = info;
		auto it = bus->route.begin();
		if (it != bus->route.end() && it + 1 != bus->route.end()){
			for (; it + 1 != bus->route.end(); ++it) {
				double time = static_cast<double>(bus_wait_time_);
				
				for (auto next_vertex = it + 1; next_vertex != bus->route.end(); ++next_vertex){
					time += tc.GetDistance(*prev(next_vertex), *next_vertex) / bus_velocity;
					edges_[graph_.AddEdge({ (*it)->vertex_id,(*next_vertex)->vertex_id, time })]
												  ={ *it,bus,static_cast<int>(next_vertex - it)};
				}	
			}
		}
	}
	router_ = std::make_unique<graph::Router<double>>(graph_);
}

std::optional<CompletedRoute> TransportRouter::ComputeRoute(graph::VertexId from, graph::VertexId to){

	std::optional<graph::Router<double>::RouteInfo> build_route = router_->BuildRoute(from, to);
	if (!build_route) { return std::nullopt;}
	if (build_route->weight < EPSILON) { return CompletedRoute({ 0, {} });}

	CompletedRoute result;
	result.total_time = build_route->weight;
	result.route.reserve(build_route->edges.size());

	for (auto& edge : build_route->edges){
		EdgeInfo& info = edges_.at(edge);
		double wait_time = static_cast<double>(bus_wait_time_);
		double run_time = graph_.GetEdge(edge).weight - bus_wait_time_;
		result.route.push_back(CompletedRoute::Line{info.stop, info.bus, wait_time, run_time, info.count});
	}
	return result;
}

} // namespace Router
} // namespace TransportCatalogue