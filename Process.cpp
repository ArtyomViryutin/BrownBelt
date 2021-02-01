#include "Process.h"
using namespace std;

vector<ResponseHolder> ProcessRequests(const vector<RequestHolder>& requests, shared_ptr<BusManager> manager) {
	vector<ResponseHolder> responses;
	responses.reserve(2000);
	for (const auto& request_holder : requests) {
		if (request_holder->type == Request::Type::PROCESS_BUS || 
			request_holder->type == Request::Type::PROCESS_STOP ||
			request_holder->type == Request::Type::PROCESS_ROUTE) {
			const auto& request = static_cast<const ProcessRequest&>(*request_holder);
			responses.push_back(request.Process(manager));
		}
		else {
			const auto& request = static_cast<const InputRequest&>(*request_holder);
			request.Process(manager);
		}
	}
	return responses;
}
