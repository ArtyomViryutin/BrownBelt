#pragma once
#include "BusManager.h"
#include "Json.h"
#include "Response.h"
#include "GraphBuilder.h"
#include <memory>
#include <unordered_map>	
#include <string_view>
#include <string>
#include <istream>
#include "profile.h"
#include <queue>
#include "dijkstra.h"

struct Request;
using RequestHolder = std::unique_ptr<Request>;

struct Request {
	enum struct Type {
		ADD_STOP, ADD_BUS, ADD_SETTINGS, PROCESS_BUS, PROCESS_STOP, PROCESS_ROUTE
	};
	Request(Type type) : type(type) {}
	static RequestHolder CreateRequest(Type type);

	virtual void ParseFrom(const Json::Node& request_node) = 0;
	virtual ~Request() = default;
	const Type type;
};

const std::unordered_map<std::string_view, Request::Type> STR_TO_INPUT_REQUEST_TYPE = {
	{"Stop", Request::Type::ADD_STOP},
	{"Bus", Request::Type::ADD_BUS},
	{"Settings", Request::Type::ADD_SETTINGS}
};
const std::unordered_map<std::string_view, Request::Type> STR_TO_PROCESS_REQUEST_TYPE = {
	{"Bus", Request::Type::PROCESS_BUS},
	{"Stop", Request::Type::PROCESS_STOP},
	{"Route", Request::Type::PROCESS_ROUTE}
};


struct InputRequest : Request {
	using Request::Request;
	virtual void Process(std::shared_ptr<BusManager> manager_ptr) const = 0;
};

struct AddStopRequest : InputRequest {
	AddStopRequest();
	void ParseFrom(const Json::Node& request_node) override;
	void Process(std::shared_ptr<BusManager> manager_ptr) const override;
	Stop stop;
	Coordinate coordinate;
	std::optional<DistancesToStops> distances_to_stops;
};

struct AddBusRequest : InputRequest {
	AddBusRequest();
	void ParseFrom(const Json::Node& request_node) override;
	void Process(std::shared_ptr<BusManager> manager_ptr) const override;
	BusId id;
	Route route;
	RouteType route_type;
};

struct AddSettingsRequest : InputRequest {
	AddSettingsRequest();
	void ParseFrom(const Json::Node& request_node) override;
	void Process(std::shared_ptr<BusManager> manager_ptr) const override;
	int bus_wait_time;
	double bus_velocity;
};

struct ProcessRequest : Request {
	using Request::Request;
	virtual ResponseHolder Process(std::shared_ptr<BusManager> manager_ptr) const = 0;
	size_t request_id = 0;
};

struct ProcessBusRequest : ProcessRequest {
	ProcessBusRequest();
	void ParseFrom(const Json::Node& request_node) override;
	ResponseHolder Process(std::shared_ptr<BusManager> manager_ptr) const override;
	BusId id;
};

struct ProcessStopRequest : ProcessRequest {
	ProcessStopRequest();
	void ParseFrom(const Json::Node& request_node) override;
	ResponseHolder Process(std::shared_ptr<BusManager> manager_ptr) const override;
	Stop stop;
};

struct ProcessRouteRequest : ProcessRequest {
	ProcessRouteRequest();
	void ParseFrom(const Json::Node& request_node) override;
	ResponseHolder Process(std::shared_ptr<BusManager> manager_ptr) const override;
	Stop stop_from;
	Stop stop_to;

};
