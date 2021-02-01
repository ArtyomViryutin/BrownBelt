#pragma once
#include <vector>
#include "Request.h"
#include "BusManager.h"

std::vector<ResponseHolder> ProcessRequests(const std::vector<RequestHolder>& requests, 
	std::shared_ptr<BusManager> manager = nullptr);
