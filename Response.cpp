#include "Response.h"

using namespace std;
Response::Response(size_t request_id, ResponseStatus status) : request_id_(request_id), status_(status) {}

BusResponse::BusResponse(size_t request_id, ResponseStatus status, int route_length, double curvature, size_t stop_count,
	size_t unique_stop_count) :
	Response(request_id, status), route_length_(route_length), curvature_(curvature),
	stop_count_(stop_count), unique_stop_count_(unique_stop_count) {}


string BusResponse::PrintResponse() const {
	ostringstream output;
	output << "{";
	output << Response::REQUEST_ID << ": " << request_id_ << ", ";
	if (status_ == ResponseStatus::UNSUCCESS) {
		output << Response::ERROR_MESSAGE << ": " << Response::NOT_FOUND << "}";
	}
	else {
		output << Response::ROUTE_LENGTH << ": " << route_length_ << ", "
			<< Response::CURVATURE << ": " << curvature_ << ", "
			<< Response::STOP_COUNT << ": " << stop_count_ << ", "
			<< Response::UNIQUE_STOP_COUNT << ": " << unique_stop_count_  << "}";
	}
	return output.str();
}


StopResponse::StopResponse(size_t request_id, ResponseStatus status, Buses buses) : Response(request_id, status), buses_(buses) {}

string StopResponse::PrintResponse() const {
	ostringstream output;
	output << "{";
	output << Response::REQUEST_ID << ": " << request_id_ << ", ";
	if (status_ == ResponseStatus::UNSUCCESS) {
		output  << Response::ERROR_MESSAGE << ": " << Response::NOT_FOUND;
	}
	else {
		output << Response::BUSES << ": " << "[";
		if (buses_.empty()) {
			output << "]";
		}
		else {
			for (auto begin = buses_.begin(); begin != buses_.end(); ++begin) {
				output << "\"" << *begin << "\"";
				if (begin != prev(buses_.end())) {
					output << ", ";
				}
			}
			output << "]";
		}
	}
	output << "}";
	return output.str();
}
RouteResponse::RouteResponse(size_t request_id, ResponseStatus status, std::vector<ItemHolder> items,
	double total_time) :
	Response(request_id, status), total_time_(total_time), items_(move(items)) {}

string RouteResponse::PrintResponse() const {
	ostringstream output;
	output << "{";
	output << Response::REQUEST_ID << ": " << request_id_ << ", ";
	if (status_ == ResponseStatus::UNSUCCESS) {
		output << Response::ERROR_MESSAGE << ": " << Response::NOT_FOUND;
	}
	else {
		output << Response::TOTAL_TIME << ": " << total_time_ << ", ";
		output << Response::ITEMS << ": [";
		if (items_.begin() == items_.end()) {
			output << "]";
		}
		else {
			for (auto item = items_.begin(); item != items_.end(); ++item) {
				output << (*item)->Print();
				if (item != prev(items_.end())) {
					output << ", ";
				}
			}
			output << "]";
		}
	}
	output << "}";
	return output.str();
}


Item::Item(double time, string type, string name) : time_(time), type_(type), name_(name) {}

WaitItem::WaitItem(double time, string stop_name) : Item(time, "Wait", stop_name) {}
string WaitItem::Print() const {
	ostringstream output;
	output << "{\"time\": " << time_ << ", " <<
		"\"stop_name\": " << "\"" << name_ << "\", " <<
		"\"type\": " << "\"" << type_ << "\"}";
	return output.str();
}


BusItem::BusItem(double time, string bus, int span_count) : Item(time, "Bus", bus), span_count_(span_count) {}
string BusItem::Print() const {
	ostringstream output;
	output << "{\"time\": " << time_ << ", " <<
		"\"bus\": " << "\"" << name_ << "\", " <<
		"\"span_count\": " << span_count_ << ", " <<
		"\"type\": " << "\"" << type_ << "\"}";
	return output.str();
}



void PrintResponses(const vector<ResponseHolder>& responses, ostream& out) {
	out << "[";
	for (auto response = responses.begin(); response != responses.end(); ++response) {
		out << response->get()->PrintResponse();
		if (response != prev(responses.end())) {
			out << ", ";
		}
	}
	out << "]";
}
