#pragma once
#include <string_view>
#include <optional>
#include <iostream>
#include <vector>
#include "Request.h"
#include "Json.h"
#include <algorithm>
#include <optional>

std::optional<Request::Type> ConvertRequestTypeFromString(std::string_view type_str, 
	const std::unordered_map<std::string_view, Request::Type>& types);
RequestHolder ParseRequest(const Json::Node& request_node, bool is_stat_request = false);
std::vector<RequestHolder> ReadFromJson(std::istream& input = std::cin);