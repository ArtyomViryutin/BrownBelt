#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <set>
#include <optional>
#include <iostream>
#include <cmath>
#include <unordered_set>
struct Edge {
	size_t from = 0;
	size_t to = 0;
	double weight = 0;
	std::string bus = "";
};
struct Coordinate {
	double latitude = 0;
	double longtitude = 0;
	std::pair<double, double> Radians() const;
};

using Stop = std::string;
using BusId = std::string;
using Route = std::vector<std::string>;
using Buses = std::set<std::string>;
using DistancesToStops = std::unordered_map<Stop, int>;

enum struct RouteType {
	LOOPING, STRIGHT
};

struct RouteInfo {
	Route route;
	RouteType type = RouteType::STRIGHT;
};

struct StopInfo {
	Coordinate coordinate;
	Buses buses;

};

struct Settings {
	int bus_wait_time = 0;
	double bus_velocity = 0;
};
struct EdgeInfo {
	size_t from;
	size_t to;
	std::string type;
	std::string bus;
};
struct BusManager {


	void CreateGraph(std::pair<std::vector<std::vector<std::pair<int, int>>>, int> graph_);
	std::vector<Edge> GetEdges(std::vector<int> path);




	BusManager() {}
	BusManager(BusManager&) = delete;
	BusManager(BusManager&&) = delete;
	BusManager operator=(BusManager&) = delete;
	BusManager operator=(BusManager&&) = delete;

	void AddStop(Stop stop, Coordinate coordinate, std::optional<DistancesToStops> distances_to_stops);
	void AddBus(BusId id, Route route, RouteType route_type);
	void AddSettings(int bus_wait_time, double bus_velocity);


	size_t StopsOnRoute(BusId id) const;
	size_t UniqueStopsOnRoute(BusId id) const;
	double ComputeCurvature(BusId id) const;
	std::optional<const Buses*> BusesOnStop(const std::string& stop) const;
	bool IsBusExists(BusId id) const;
	int RoadLength(BusId id) const;
	double GeographicLength(BusId id) const;
	int GetDistanceBtwStops(Stop from, Stop to) const;

	size_t GetStopIndex(Stop stop) const;
	Stop GetStopByIndex(size_t) const;


	void AddEdge(Edge edge);
	Settings settings;
	std::unordered_map<BusId, RouteInfo> buses_;
	std::unordered_map<Stop, StopInfo> stops_;
	std::unordered_map<Stop, DistancesToStops> distances_btw_stops;
	
	mutable std::unordered_map<Stop, size_t> outers;
	mutable std::unordered_map<size_t, Stop> indexes;


	std::unordered_map<size_t, std::unordered_map<size_t, Edge>> edges;


	std::pair<std::vector<std::vector<std::pair<int, int>>>, int> graph;
	std::vector<std::pair<std::vector<int>, std::vector<int>>> weights_and_parents;


	void AddDistances(const Stop& stop_from, const DistancesToStops& distances_to_stops);
	void AddDistance(const Stop& stop_from, const Stop& stop_to, int distance);
	int ComputeRoadLength(const Stop& src, const Stop& dst) const;
	double ComputeGeographicLength(const Stop& src, const Stop& dst) const;
};
