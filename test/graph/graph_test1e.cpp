#include "gdwg/graph.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// Collection 1e - Sundry
TEST_CASE("extractor prints empty string for empty graph") {
	auto g = gdwg::graph<int, int>();
	auto oss = std::ostringstream{};
	oss << g;
	CHECK(oss.str().empty());
}
TEST_CASE("begin == end the graph is empty") {
	auto g = gdwg::graph<int, std::set<int>>{1, 2, 3};
	CHECK(g.begin() == g.end());

	auto const gc = gdwg::graph<std::string, std::set<int>>{};
	CHECK(gc.begin() == gc.end());
}
TEST_CASE("extractor prints a small graph according to the example provided") {
	using graph = gdwg::graph<int, int>;
	auto const v = std::vector<graph::value_type>{
	   {4, 1, -4},
	   {3, 2, 2},
	   {2, 4, 2},
	   {2, 1, 1},
	   {6, 2, 5},
	   {6, 3, 10},
	   {1, 5, -1},
	   {3, 6, -8},
	   {4, 5, 3},
	   {5, 2, 7},
	};

	auto g = graph{};
	for (const auto& [from, to, weight] : v) {
		g.insert_node(from);
		;
		g.insert_node(to);
		g.insert_edge(from, to, weight);
	}

	g.insert_node(64);
	auto out = std::ostringstream{};
	out << g;
	auto const expected_output = std::string_view(R"(1 (
  5 | -1
)
2 (
  1 | 1
  4 | 2
)
3 (
  2 | 2
  6 | -8
)
4 (
  1 | -4
  5 | 3
)
5 (
  2 | 7
)
6 (
  2 | 5
  3 | 10
)
64 (
)
)");
	CHECK(out.str() == expected_output);
}
TEST_CASE("Two graphs created separately return true for == operator iff all nodes and edges "
          "match") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, 4));

	auto g2 = gdwg::graph<int, int>{3, 2, 1};
	CHECK(not(g == g2));
	CHECK(g2.insert_edge(1, 2, 4));
	CHECK(g == g2);
	CHECK(g2.erase_node(3));
	CHECK(not(g == g2));
}
