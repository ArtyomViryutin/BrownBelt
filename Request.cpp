#include "Request.h"
using namespace std;

RequestHolder Request::CreateRequest(Request::Type type) {
	switch (type)
	{
	case Request::Type::ADD_STOP:
		return make_unique<AddStopRequest>();
		break;
	case Request::Type::ADD_BUS:
		return make_unique<AddBusRequest>();
		break;
	case Request::Type::ADD_SETTINGS:
		return make_unique<AddSettingsRequest>();
	case Request::Type::PROCESS_BUS:
		return make_unique<ProcessBusRequest>();
		break;
	case Request::Type::PROCESS_STOP:
		return make_unique<ProcessStopRequest>();
		break;
	case Request::Type::PROCESS_ROUTE:
		return make_unique<ProcessRouteRequest>();
		break;
	default:
		break;
	}
}

AddStopRequest::AddStopRequest() : InputRequest(Request::Type::ADD_STOP), stop("") {}
void AddStopRequest::Process(shared_ptr<BusManager> manager_ptr) const {
	manager_ptr->AddStop(stop, coordinate, distances_to_stops);
}
void AddStopRequest::ParseFrom(const Json::Node& request_node) {
	stop = request_node.AsMap().at("name").AsString();
	coordinate.latitude = request_node.AsMap().at("latitude").AsDouble();
	coordinate.longtitude = request_node.AsMap().at("longitude").AsDouble();
	DistancesToStops distances;
	auto& road_distances = request_node.AsMap().at("road_distances").AsMap();
	if (!road_distances.empty()) {
		for (const auto& [stop, distance] : road_distances) {
			distances[stop] = static_cast<int>(distance.AsDouble());
		}
		distances_to_stops = distances;
	}
	else {
		distances_to_stops = nullopt;
	}
}
AddBusRequest::AddBusRequest() : InputRequest(Request::Type::ADD_BUS), id(""), route_type(RouteType::STRIGHT) {}
void AddBusRequest::Process(shared_ptr<BusManager> manager_ptr) const {
	manager_ptr->AddBus(id, route, route_type);
}
void AddBusRequest::ParseFrom(const Json::Node& request_node) {
	id = request_node.AsMap().at("name").AsString();
	if (request_node.AsMap().at("is_roundtrip").AsBool()) {
		route_type = RouteType::LOOPING;
	}
	for (const auto& stop : request_node.AsMap().at("stops").AsArray()) {
		route.push_back(stop.AsString());
	}
}

AddSettingsRequest::AddSettingsRequest() : InputRequest(Request::Type::ADD_SETTINGS), bus_velocity(0), bus_wait_time(0) {}
void AddSettingsRequest::ParseFrom(const Json::Node& request_node) {
	bus_wait_time = static_cast<int>(request_node.AsMap().at("bus_wait_time").AsDouble());
	bus_velocity = request_node.AsMap().at("bus_velocity").AsDouble();


}
void AddSettingsRequest::Process(shared_ptr<BusManager> manager_ptr) const {
	manager_ptr->AddSettings(bus_wait_time, bus_velocity * 100.0 / 6.0);
}





ProcessBusRequest::ProcessBusRequest() : ProcessRequest(Request::Type::PROCESS_BUS), id("") {}
void ProcessBusRequest::ParseFrom(const Json::Node& request_node) {
	id = request_node.AsMap().at("name").AsString();
	request_id = static_cast<size_t>(request_node.AsMap().at("id").AsDouble());
}
ResponseHolder ProcessBusRequest::Process(shared_ptr<BusManager> manager_ptr) const {
	auto& manager = *manager_ptr;
	if (!manager.IsBusExists(id)) {
		return make_unique<BusResponse>(request_id, Response::ResponseStatus::UNSUCCESS);
	}
	return make_unique<BusResponse>(request_id, Response::ResponseStatus::SUCCESS, manager.RoadLength(id), manager.ComputeCurvature(id),
		manager.StopsOnRoute(id), manager.UniqueStopsOnRoute(id));
}

ProcessStopRequest::ProcessStopRequest() : ProcessRequest(Request::Type::PROCESS_STOP), stop("") {}
void ProcessStopRequest::ParseFrom(const Json::Node& request_node) {
	stop = request_node.AsMap().at("name").AsString();
	request_id = static_cast<size_t>(request_node.AsMap().at("id").AsDouble());
}

ResponseHolder ProcessStopRequest::Process(shared_ptr<BusManager> manager_ptr) const {
	const auto buses = manager_ptr->BusesOnStop(stop);
	if (!buses) {
		return make_unique<StopResponse>(request_id, Response::ResponseStatus::UNSUCCESS);
	}
	else {
		return make_unique<StopResponse>(request_id, Response::ResponseStatus::SUCCESS, *buses.value());
	}
}

ProcessRouteRequest::ProcessRouteRequest() : ProcessRequest(Request::Type::PROCESS_ROUTE),
stop_from(""), stop_to("") {}

void ProcessRouteRequest::ParseFrom(const Json::Node& request_node) {
	stop_from = request_node.AsMap().at("from").AsString();
	stop_to = request_node.AsMap().at("to").AsString();
	request_id = static_cast<size_t>(request_node.AsMap().at("id").AsDouble());
}



ItemHolder AddWait(double time, string stop_name) {
	return make_unique<WaitItem>(time, stop_name);
}

ItemHolder AddBus(double time, string bus, int span_count) {
	return make_unique<BusItem>(time, bus, span_count);
}
ResponseHolder ProcessRouteRequest::Process(shared_ptr<BusManager> manager_ptr) const {
	auto& manager = *manager_ptr.get();
	if (manager.stops_.count(stop_from) == 0 || manager.stops_.count(stop_to) == 0) {
		return make_unique<RouteResponse>(request_id, Response::ResponseStatus::UNSUCCESS);
	}
	else {
		vector<ItemHolder> items;
		if (manager.graph.second == 0) {
			manager.CreateGraph(BuildGraph(manager));
		}
		const auto from = manager.outers[stop_from];
		const auto to = manager.outers[stop_to];
		if (manager.weights_and_parents[from].first.empty()) {
			manager.weights_and_parents[from] = Dijkstra(from, manager.graph);
		}
		if (manager.weights_and_parents[from].first[to] == 2'000'000'000) {
			return make_unique<RouteResponse>(request_id, Response::ResponseStatus::UNSUCCESS);
		}
		vector<Edge> edges = manager.GetEdges(GetPath(from, to, manager));

		for (size_t i = 0; i < edges.size(); ++i) {
			if (edges[i].bus.empty()) {
				items.push_back(AddWait(edges[i].weight / manager.settings.bus_velocity,
					manager.GetStopByIndex(edges[i].from)));
			}
			else {
				auto [from, to, w, bus] = edges[i];
				double weight = 0;
				int span_count = 0;
				for (; i < edges.size() && (edges[i].bus == bus ||
					edges[i].weight == 0); ++i) {
					++span_count;
					weight += edges[i].weight;
					to = edges[i].to;
				}
				items.push_back(AddBus(weight / manager.settings.bus_velocity, bus, span_count - 1));
				--i;
			}
		}
		return make_unique<RouteResponse>(request_id, Response::ResponseStatus::SUCCESS, move(items),
			manager.weights_and_parents[from].first[to] / manager.settings.bus_velocity);
	}
}