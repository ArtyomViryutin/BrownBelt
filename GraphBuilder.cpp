#include "GraphBuilder.h"
using namespace std;

size_t GraphBuilder::GetNewVertexIndex() {
	return vertex_count++;
}


void GraphBuilder::AddOuterVertexIndexes(const std::unordered_map<Stop, StopInfo>& stops_,
	const BusManager& manager) {
	for (const auto& [stop, stop_info] : stops_) {
		manager.outers[stop] = GetNewVertexIndex();
		manager.indexes[manager.outers[stop]] = stop;
	}
}
void GraphBuilder::AddInnerIndexesForBusRoute(const std::string& bus, const std::vector<std::string>& stops,
	const BusManager& manager) {
	for (const auto& stop : stops) {
		inner_indexes_for_bus_route[bus].push_back({ stop, GetNewVertexIndex() });
		manager.indexes[inner_indexes_for_bus_route[bus].back().index] = stop;
	}
}
std::vector<Edge> GraphBuilder::AddOuterAndInnerEdgesForStrightRoute(std::string bus, const BusManager& manager) {
	std::vector<Edge> edges;
	auto& stops = inner_indexes_for_bus_route[bus];
	edges.reserve(stops.size() * 2);
	for (const auto& [stop, stop_index] : stops) {
		edges.push_back({ manager.outers[stop], stop_index, manager.settings.bus_wait_time
			* manager.settings.bus_velocity, "" });
		edges.push_back({ stop_index, manager.outers[stop], 0, "" });
	}
	return edges;
}
std::vector<Edge> GraphBuilder::AddOuterAndInnerEdgesForLoopingRoute(std::string bus, const BusManager& manager) {
	std::vector<Edge> edges;
	auto& stops = inner_indexes_for_bus_route[bus];
	edges.reserve(stops.size());
	for (size_t i = 0; i < stops.size() - 1; ++i) {
		edges.push_back({ manager.outers[stops[i].stop], stops[i].index,
			manager.settings.bus_wait_time * manager.settings.bus_velocity, "" });
		edges.push_back({ stops[i].index, manager.outers[stops[i].stop], 0, "" });
	}
	edges.push_back({stops.back().index, manager.outers[stops.back().stop],  0, "" });
	return edges;
}

std::vector<Edge> GraphBuilder::AddLoopingRoute(std::string bus, const BusManager& manager) {
	std::vector<Edge> edges;
	auto& stops = inner_indexes_for_bus_route[bus];
	edges.reserve(stops.size());
	for (size_t i = 0; i < stops.size() - 1; ++i) {
		edges.push_back({ stops[i].index, stops[i + 1].index,
			 static_cast<double>(manager.GetDistanceBtwStops(stops[i].stop, stops[i + 1].stop)), bus });
	}
	return edges;
}

std::vector<Edge> GraphBuilder::AddStrightRoute(std::string bus, const BusManager& manager) {
	std::vector<Edge> edges;
	auto& stops = inner_indexes_for_bus_route[bus];
	edges.reserve(stops.size() * 2);
	for (size_t i = 0; i < stops.size() - 1; ++i) {
		edges.push_back({ stops[i].index, stops[i + 1].index,
			 static_cast<double>(manager.GetDistanceBtwStops(stops[i].stop, stops[i + 1].stop)), bus });
	}
	for (size_t i = stops.size(); i > 1; --i) {
		edges.push_back({ stops[i - 1].index, stops[i - 2].index,
			 static_cast<double>(manager.GetDistanceBtwStops(stops[i - 1].stop, stops[i - 2].stop)), bus });
	}
	return edges;
}

std::pair<std::vector<std::vector<std::pair<int, int>>>, int> BuildGraph(BusManager& manager) {
	GraphBuilder builder;
	builder.AddOuterVertexIndexes(manager.stops_, manager);
	
	std::vector<std::vector<Edge>> edges;
	for (const auto& [bus, bus_info] : manager.buses_) {
		builder.AddInnerIndexesForBusRoute(bus, bus_info.route, manager);
		for (auto i : manager.outers) {
		}
		if (bus_info.type == RouteType::LOOPING) {
			edges.push_back(builder.AddOuterAndInnerEdgesForLoopingRoute(bus, manager));
			edges.push_back(builder.AddLoopingRoute(bus, manager));
		}
		else {
			edges.push_back(builder.AddOuterAndInnerEdgesForStrightRoute(bus, manager));
			edges.push_back(builder.AddStrightRoute(bus, manager));
		}
	}
	std::vector<std::vector<std::pair<int, int>>> graph(builder.vertex_count);
	for (const auto& route_edges : edges) {
		for (const auto& edge : route_edges) {
			graph[edge.from].push_back(std::make_pair(edge.weight, edge.to));
			manager.edges[edge.from][edge.to] = edge;
		}
	}
	return { move(graph), builder.vertex_count };
}

