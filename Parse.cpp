#include "Parse.h"
using namespace std;


std::optional<Request::Type> ConvertRequestTypeFromString(std::string_view type_str,
	const std::unordered_map<std::string_view, Request::Type>& types) {
	if (const auto it = types.find(type_str);
		it != types.end()) {
		return it->second;
	}
	else {
		return nullopt;
	}
}

RequestHolder ParseRequest(const Json::Node& request_node, bool is_stat_request) {
	string_view request_type_str;
	if (request_node.AsMap().count("type") == 0) {
		request_type_str = "Settings";
	}
	else {
		request_type_str = request_node.AsMap().at("type").AsString();
	}
	const auto request_type = is_stat_request ?
		ConvertRequestTypeFromString(request_type_str, STR_TO_PROCESS_REQUEST_TYPE) :
		ConvertRequestTypeFromString(request_type_str, STR_TO_INPUT_REQUEST_TYPE);
	if (!request_type) {
		return nullptr;
	}
	RequestHolder request = Request::CreateRequest(request_type.value());
	if (request) {
		request->ParseFrom(request_node);
	}
	return request;
}
void ReadRequestsOfCertainType(vector<RequestHolder>* formed_requests, string_view type,
	const Json::Node& requests) {
	for (const auto& request_node : requests.AsArray()) {
		if (auto request = ParseRequest(request_node, type == "stat_requests" ? true : false)) {
			formed_requests->push_back(move(request));
		}
	}
}
vector<RequestHolder> ReadFromJson(istream& input) {
	const Json::Document all_requests = Json::Load(input);
	const auto& settings = all_requests.GetRoot().AsMap().at("routing_settings");
	const auto& base_requests = all_requests.GetRoot()
		.AsMap().at("base_requests");
	const auto& stat_requests = all_requests.GetRoot()
		.AsMap().at("stat_requests");
	vector<RequestHolder> formed_requests;
	formed_requests.reserve(base_requests.AsArray().size() + stat_requests.AsArray().size() + 1);
	formed_requests.push_back(ParseRequest(settings));
	ReadRequestsOfCertainType(&formed_requests, "base_requests", base_requests);
	ReadRequestsOfCertainType(&formed_requests, "stat_requests", stat_requests);
	return formed_requests;
}