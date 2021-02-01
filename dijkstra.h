#pragma once
#include <vector>
#include <queue>
#include <optional>
#include "BusManager.h"

std::pair<std::vector<int>, std::vector<int>> Dijkstra(size_t from,
	const std::pair<std::vector<std::vector<std::pair<int, int>>>, int>& graph);
std::vector<int> GetPath(size_t from, size_t to, BusManager& manager);
