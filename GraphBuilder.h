#pragma once
#include <unordered_map>
#include "BusManager.h"
#include <memory>
using Indexes = std::unordered_map<std::string, size_t>;



struct GraphBuilder {
	struct SInfo {
		std::string stop;
		size_t index;
	};

	void AddOuterVertexIndexes(const std::unordered_map<Stop, StopInfo>& stops, const BusManager& manager);
	void AddInnerIndexesForBusRoute(const std::string& bus, const std::vector<std::string>& stops,
		const BusManager& manager);

	std::vector<Edge> AddLoopingRoute(std::string bus, const BusManager& manager);

	std::vector<Edge> AddStrightRoute(std::string bus, const BusManager& manager);

	std::vector<Edge> AddOuterAndInnerEdgesForLoopingRoute(std::string bus, const BusManager& manager);
	std::vector<Edge> AddOuterAndInnerEdgesForStrightRoute(std::string bus, const BusManager& manager);

	size_t GetNewVertexIndex();

	std::unordered_map<std::string, std::vector<SInfo>> inner_indexes_for_bus_route;
	size_t vertex_count = 0;
};
std::pair<std::vector<std::vector<std::pair<int, int>>>, int> BuildGraph(BusManager& manager);