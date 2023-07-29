#pragma once
#include "graph.h"
#include "ranges.h"
#include "router.h"
#include "domain.h"
#include "transport_catalogue.h"

#include <map>

const double EPSILON = 1e-6;
const double M_MMIN = 1000 * 1.0 / 60;

namespace TransportCatalogue{
namespace Router {

using namespace std::string_literals;

struct EdgeInfo{
	const Stop* stop;
	const Bus* bus;
	int count;
};

struct CompletedRoute {
	struct Line {
		const Stop* stop;
		const Bus* bus;
		double wait_time;
		double run_time;
		int count_stops;
	};
	double total_time;
	std::vector<Line> route;
};

class TransportRouter {
public:
	std::optional<CompletedRoute> ResultRoute(const TransportCatalogue& tc, 
							const std::string& from, const std::string& to);
	void SetBusWaitTime(int time);
	void SetBusSpeed(double speed);
	std::unordered_map<std::string, graph::VertexId> GetStopIds() const;
	std::pair<int, double> GetRouterSettings() const;
	graph::DirectedWeightedGraph<double> GetGraph() const;
	void SetStopIds(const std::unordered_map<std::string, graph::VertexId>& stop_ids);
	void SetGraph(const graph::DirectedWeightedGraph<double> &graph);

private:
	int bus_wait_time_ = 0;
	double bus_speed_ = 0.0;
	graph::VertexId vertex_count_ = 0;
	graph::DirectedWeightedGraph<double> graph_;
	std::unordered_map<graph::EdgeId, EdgeInfo> edges_;
	std::unique_ptr<graph::Router<double>> router_;
	std::unordered_map<std::string, graph::VertexId> vertex_ids;
	
	void CreateGraph(const std::unordered_map <std::string_view, Bus*>& info_bus, const TransportCatalogue& tc);
	std::optional<CompletedRoute> ComputeRoute(graph::VertexId from, graph::VertexId to);
	void SetVertextIDs(const TransportCatalogue& tc);
};
} //namespace Router
} //namespace TransportCatalogue