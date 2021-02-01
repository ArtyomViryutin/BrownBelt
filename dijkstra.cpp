#include "dijkstra.h"
using namespace std;


vector<int> GetPath(size_t from, size_t to, BusManager& manager) {
	vector<int> path;
	path.push_back(to);
	while (to != from) {
		to = manager.weights_and_parents[from].second[to];
		path.push_back(to);
	}
	return path;
}
std::pair<std::vector<int>, std::vector<int>> Dijkstra(size_t from,
	const std::pair<std::vector<std::vector<std::pair<int, int>>>, int>& graph) {
	auto& [g, n] = graph;
	vector<int> d(n, 2'000'000'000), p(n);
	d[from] = 0;
	priority_queue<pair<int, int>> q;
	q.push(make_pair(0, from));
	while (!q.empty()) {
		int v = q.top().second, cur_d = -q.top().first;
		q.pop();
		if (cur_d > d[v])  continue;
		for (size_t j = 0; j < g[v].size(); ++j) {
			int to = g[v][j].second,
				len = g[v][j].first;
			if (d[v] + len < d[to]) {
				d[to] = d[v] + len;
				p[to] = v;
				q.push(make_pair(-d[to], to));
			}
		}
	}
	return {move(d), move(p)};
}