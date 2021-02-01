#include "BusManager.h"

using namespace std;
pair<double, double> Coordinate::Radians() const {
	return { latitude * 3.1415926535 / 180.0,
		longtitude * 3.1415926535 / 180.0 };
}
double BusManager::ComputeGeographicLength(const Stop& src, const Stop& dst) const {
	auto [lhs_lat, lhs_lon] = stops_.at(src).coordinate.Radians();
	auto [rhs_lat, rhs_lon] = stops_.at(dst).coordinate.Radians();
	return acos(sin(lhs_lat) * sin(rhs_lat) +
				cos(lhs_lat) * cos(rhs_lat) *
				cos(abs(lhs_lon - rhs_lon))
	) * 6371000;
}
int BusManager::ComputeRoadLength(const Stop& src, const Stop& dst) const {
	return distances_btw_stops.at(src).at(dst);
}


double BusManager::GeographicLength(BusId id) const {
	auto& [route, route_type] = buses_.at(id);
	double route_lenght = 0;
	for (size_t i = 0; i < route.size() - 1; ++i) {
		route_lenght += ComputeGeographicLength(route[i], route[i + 1]);
	}
	return route_type == RouteType::LOOPING ? route_lenght : 2 * route_lenght;
}

int BusManager::RoadLength(BusId id) const {
	auto& [route, route_type] = buses_.at(id);
	int route_length = 0;
	for (size_t i = 0; i < route.size() - 1; ++i) {
		route_length += ComputeRoadLength(route[i], route[i + 1]);
	}
	if (route_type == RouteType::LOOPING) {
		route_length;
	}
	else {
		for (size_t i = route.size(); i > 1; --i) {
			route_length += ComputeRoadLength(route[i - 1], route[i - 2]);
		}
	}
	return route_length;
}
void BusManager::AddDistance(const Stop& stop_from, const Stop& stop_to, int distance) {
	distances_btw_stops[stop_from][stop_to] = distance;
	if (distances_btw_stops[stop_to][stop_from] == 0) {
		distances_btw_stops[stop_to][stop_from] = distance;
	}
}
void BusManager::AddDistances(const Stop& stop_from, const DistancesToStops& distances_to_stops) {
	for (const auto& [stop_to, distance] : distances_to_stops) {
		AddDistance(stop_from, stop_to, distance);
	}
}
void BusManager::AddStop(Stop stop, Coordinate coordinate, std::optional<DistancesToStops> stops_distances) {
	stops_[stop].coordinate = coordinate;
	if (stops_distances) {
		AddDistances(stop, stops_distances.value());
	}
} 
void BusManager::AddBus(BusId id, Route route, RouteType route_type) {
	for (const auto& stop : route) {
		stops_[stop].buses.insert(id);
	}
	buses_[id] = { route, route_type };
}
void BusManager::AddSettings(int bus_wait_time, double bus_velocity) {
	settings.bus_wait_time = bus_wait_time;
	settings.bus_velocity = bus_velocity;
}

bool BusManager::IsBusExists(BusId id) const {
	return buses_.count(id) == 1;
}



double BusManager::ComputeCurvature(BusId id) const {
	return 1.0 * RoadLength(id) / GeographicLength(id);
}
size_t BusManager::StopsOnRoute(BusId id) const {
	auto& [route, route_type] = buses_.at(id);
	if (route_type == RouteType::STRIGHT) {
		return 2 * distance(route.begin(), route.end()) - 1;
	}
	else {
		return distance(route.begin(), route.end());
	}
}
size_t BusManager::UniqueStopsOnRoute(BusId id) const {
	auto& [route, route_type] = buses_.at(id);
	return set<string_view>(route.begin(), route.end()).size();
}
optional<const Buses*> BusManager::BusesOnStop(const string& stop) const {
	if (stops_.count(stop) == 0) {
		return nullopt;
	}
	else {
		return &stops_.at(stop).buses;
	}
}


int BusManager::GetDistanceBtwStops(Stop from, Stop to) const {
	return distances_btw_stops.at(from).at(to);
}



size_t BusManager::GetStopIndex(Stop stop) const {
	return outers.at(stop);
}
Stop BusManager::GetStopByIndex(size_t index) const {
	return indexes[index];
}

void BusManager::AddEdge(Edge edge) {
	edges[edge.from][edge.to] = edge;
}


void BusManager::CreateGraph(std::pair<std::vector<std::vector<std::pair<int, int>>>, int> graph_) {
	graph = move(graph_);
	weights_and_parents.resize(graph.second);
}



std::vector<Edge> BusManager::GetEdges(std::vector<int> path) {
	vector<Edge> result;
	edges.reserve(path.size());
	for (int i = path.size() - 1; i > 0; --i) {

		result.push_back(edges[path[i]][path[i - 1]]);
	}
	return result;
}

