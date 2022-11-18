#include "gdwg/graph.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

// Collection 2 - Exceptions
TEST_CASE("insert edge returns exception if either node is non existant") {
	auto g = gdwg::graph<std::pair<int, int>, std::string>{{1, 1}, {2, 2}, {3, 3}};
	CHECK_THROWS_WITH(g.insert_edge({1, 1}, {2, 1}, std::string{"Z"}),
	                  "Cannot call gdwg::graph<N, E>::insert_edge when either "
	                  "src or dst node does not exist");
	CHECK_THROWS_WITH(g.insert_edge({1, 0}, {2, 2}, std::string{"Z"}),
	                  "Cannot call gdwg::graph<N, E>::insert_edge when either "
	                  "src or dst node does not exist");
}
TEST_CASE("replace node returns exception if node is non existant") {
	auto g = gdwg::graph<std::pair<int, int>, std::string>{{1, 1}, {2, 2}, {3, 3}};
	CHECK_THROWS_WITH(g.replace_node({0, 1}, {2, 1}),
	                  "Cannot call gdwg::graph<N, E>::replace_node on a node that doesn't exist");
}
TEST_CASE("merge replace node returns exception if either node is non existant") {
	auto g = gdwg::graph<std::pair<int, int>, std::string>{{1, 1}, {2, 2}, {3, 3}};
	CHECK_THROWS_WITH(g.merge_replace_node({1, 1}, {2, 1}),
	                  "Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if they "
	                  "don't exist in the graph");
	CHECK_THROWS_WITH(g.merge_replace_node({1, 0}, {2, 2}),
	                  "Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if they "
	                  "don't exist in the graph");
}

TEST_CASE("erase edge returns exception if either node is non existant") {
	auto g = gdwg::graph<int, std::string>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, "1"));
	CHECK(g.insert_edge(1, 2, "3"));
	CHECK_THROWS_WITH(g.erase_edge(4, 2, "3"),
	                  "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't exist "
	                  "in the graph");
	CHECK_THROWS_WITH(g.erase_edge(1, 4, "3"),
	                  "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't exist "
	                  "in the graph");
}
TEST_CASE("is connected returns exception if either node is non existant") {
	auto g = gdwg::graph<int, std::string>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, "1"));
	CHECK(g.insert_edge(1, 2, "3"));
	CHECK_THROWS_WITH(g.is_connected(4, 2),
	                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't exist "
	                  "in the graph");
	CHECK_THROWS_WITH(g.is_connected(1, 4),
	                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't exist "
	                  "in the graph");
}

TEST_CASE("weights returns exception if either node is non existant") {
	auto g = gdwg::graph<int, std::string>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, "1"));
	CHECK(g.insert_edge(1, 2, "3"));
	CHECK_THROWS_WITH(g.weights(4, 2),
	                  "Cannot call gdwg::graph<N, E>::weights if src or dst node don't exist in the "
	                  "graph");
	CHECK_THROWS_WITH(g.weights(1, 4),
	                  "Cannot call gdwg::graph<N, E>::weights if src or dst node don't exist in the "
	                  "graph");
}
TEST_CASE("connections returns exception if node is non existant") {
	auto g = gdwg::graph<int, std::string>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, "1"));
	CHECK(g.insert_edge(1, 2, "3"));
	CHECK_THROWS_WITH(g.connections(4),
	                  "Cannot call gdwg::graph<N, E>::connections if src doesn't exist in the "
	                  "graph");
}