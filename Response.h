#pragma once
#include <memory>
#include <optional>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include "BusManager.h"


struct Response;
struct Item;
using ResponseHolder = std::unique_ptr<Response>;
using ItemHolder = std::unique_ptr<Item>;
struct Response {
	enum struct ResponseStatus {
		SUCCESS, UNSUCCESS
	};
	Response(size_t request_id, ResponseStatus status);
	virtual std::string PrintResponse() const = 0;
	size_t request_id_;	
	const std::string ERROR_MESSAGE = "\"error_message\"";
	const std::string ROUTE_LENGTH = "\"route_length\"";
	const std::string REQUEST_ID = "\"request_id\"";
	const std::string CURVATURE = "\"curvature\"";
	const std::string STOP_COUNT = "\"stop_count\"";
	const std::string UNIQUE_STOP_COUNT = "\"unique_stop_count\"";
	const std::string NOT_FOUND = "\"not found\"";
	const std::string TOTAL_TIME = "\"total_time\"";
	const std::string BUSES = "\"buses\"";
	const std::string ITEMS = "\"items\"";
	ResponseStatus status_;
	virtual ~Response() = default;
};

struct StopResponse : Response {
	StopResponse(size_t request_id, ResponseStatus status, Buses buses = {});
	std::string PrintResponse() const override;
	Buses buses_;
};

struct BusResponse : Response {
	BusResponse(size_t request_id, ResponseStatus status, int route_length = 0, double curvature = 0,
		size_t stop_count = 0, size_t unique_stop_count = 0);
	std::string PrintResponse() const override;
	int route_length_;
	double curvature_;
	size_t stop_count_;
	size_t unique_stop_count_;
};

struct RouteResponse : Response {
	RouteResponse(size_t request_id, ResponseStatus status, std::vector<ItemHolder> items = {}, double total_time = 0);
	std::string PrintResponse() const override;
	double total_time_;
	std::vector<ItemHolder> items_;
};


struct Item {
	Item(double time, std::string type, std::string name);
	virtual std::string Print() const = 0;
	virtual ~Item() = default;
	double time_;
	std::string type_;
	std::string name_;
};
struct WaitItem : Item {
	WaitItem(double time, std::string stop_name);
	std::string Print() const override;
};

struct BusItem : Item {
	BusItem(double time, std::string bus, int span_count);
	std::string Print() const override;
	int span_count_;
};

void PrintResponses(const std::vector<ResponseHolder>& responses, std::ostream& out = std::cout);


