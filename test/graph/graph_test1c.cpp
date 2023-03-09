#include "gdwg/graph.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

// Collection 1c - Accessors
TEST_CASE("is_node returns true if and only if the node exists for a complex data type") {
	auto g = gdwg::graph<std::vector<int>, int>{std::vector<int>{1, 2}, std::vector<int>{1}};
	CHECK(g.is_node(std::vector<int>{1}));
	CHECK(g.is_node(std::vector<int>{1, 2}));
	CHECK(not g.is_node(std::vector<int>{2, 1}));

	auto const gc = std::move(g);
	CHECK(gc.is_node(std::vector<int>{1}));
	CHECK(gc.is_node(std::vector<int>{1, 2}));
	CHECK(not gc.is_node(std::vector<int>{2, 1}));
}
TEST_CASE("empty returns false if there are nodes but no edges; returns false after all nodes are "
          "deleted using erase_node") {
	auto g = gdwg::graph<int, std::set<int>>{1, 2, 3};
	CHECK(not g.empty());
	CHECK(g.erase_node(2));
	CHECK(g.erase_node(3));
	CHECK(g.erase_node(1));
	CHECK(g.empty());

	auto const gc = gdwg::graph<float, std::set<int>>{1.0, 2.0, 3.0};
	CHECK(not gc.empty());

	auto const gc2 = gdwg::graph<std::set<int>, float>{};
	CHECK(gc2.empty());
}
TEST_CASE("is_connected returns true only if an edge exists in the graph") {
	auto g = gdwg::graph<int, std::set<int>>{1, 2, 3};
	CHECK(g.insert_edge(1, 1, std::set<int>{0}));
	CHECK(g.insert_edge(1, 1, std::set<int>{0, 1}));
	CHECK(g.insert_edge(1, 2, std::set<int>{0}));
	CHECK(g.is_connected(1, 1));
	CHECK(g.is_connected(1, 2));
	CHECK(not g.is_connected(1, 3));
	CHECK(not g.is_connected(2, 1));
	CHECK(not g.is_connected(2, 2));
	CHECK(not g.is_connected(2, 3));
	CHECK(not g.is_connected(3, 1));
	CHECK(not g.is_connected(3, 2));
	CHECK(not g.is_connected(3, 3));
	auto const gc = std::move(g);
	CHECK(gc.is_connected(1, 1));
	CHECK(gc.is_connected(1, 2));
	CHECK(not gc.is_connected(1, 3));
	CHECK(not gc.is_connected(2, 1));
	CHECK(not gc.is_connected(2, 2));
	CHECK(not gc.is_connected(2, 3));
	CHECK(not gc.is_connected(3, 1));
	CHECK(not gc.is_connected(3, 2));
	CHECK(not gc.is_connected(3, 3));
}
TEST_CASE("nodes returns nodes in ascending order regardless of order it is inserted") {
	auto g = gdwg::graph<int, int>{9, -1, 4, 0};
	CHECK(g.nodes() == std::vector<int>{-1, 0, 4, 9});
	CHECK(g.insert_node(10));
	CHECK(g.nodes() == std::vector<int>{-1, 0, 4, 9, 10});
	CHECK(g.replace_node(10, -10));
	CHECK(g.nodes() == std::vector<int>{-10, -1, 0, 4, 9});
	CHECK(g.erase_node(0));
	CHECK(g.nodes() == std::vector<int>{-10, -1, 4, 9});

	auto const gc = gdwg::graph<std::string, int>{std::string{"9"},
	                                              std::string{"-1"},
	                                              std::string{"4"},
	                                              std::string{"0"}};
	CHECK(gc.nodes()
	      == std::vector<std::string>{std::string{"-1"},
	                                  std::string{"0"},
	                                  std::string{"4"},
	                                  std::string{"9"}});
}
TEST_CASE("weights returns weights in asecnding order regardless of order it is inserted") {
	auto g = gdwg::graph<int, std::string>{1, 2, 3};
	CHECK(g.insert_edge(3, 3, "2"));
	CHECK(g.insert_edge(3, 3, "4"));
	CHECK(g.insert_edge(3, 3, "1"));
	CHECK(g.insert_edge(3, 3, "3"));
	CHECK(g.weights(3, 3)
	      == std::vector<std::string>{std::string{"1"},
	                                  std::string{"2"},
	                                  std::string{"3"},
	                                  std::string{"4"}});
	auto const gc = std::move(g);
	CHECK(gc.weights(3, 3)
	      == std::vector<std::string>{std::string{"1"},
	                                  std::string{"2"},
	                                  std::string{"3"},
	                                  std::string{"4"}});
}
TEST_CASE("find returns a pointer to an equivelent edge, otherwise refers to end") {
	auto g = gdwg::graph<int, std::string>{1, 2, 3};
	CHECK(g.find(1, 2, "1") == g.end());
	CHECK(g.insert_edge(1, 2, "1"));
	CHECK(g.insert_edge(1, 2, "3"));
	auto result = g.find(1, 2, "1");
	CHECK((*result).from == 1);
	CHECK((*result).to == 2);
	CHECK((*result).weight == "1");
	CHECK(g.find(1, 2, "2") == g.end());
}
TEST_CASE("connections returns only connections leaving from src") {
	auto g = gdwg::graph<std::pair<int, int>, std::string>{{1, 1}, {2, 2}, {3, 3}};
	CHECK(g.insert_edge({2, 2}, {2, 2}, "2"));
	CHECK(g.insert_edge({2, 2}, {1, 1}, "1"));
	CHECK(g.connections({1, 1}).empty());
	CHECK(g.insert_edge({1, 1}, {2, 2}, "1"));
	CHECK(g.connections({1, 1}) == std::vector<std::pair<int, int>>{{2, 2}});
	CHECK(g.erase_edge({1, 1}, {2, 2}, "1"));
	CHECK(g.connections({1, 1}).empty());

	CHECK(g.insert_edge({1, 1}, {2, 2}, "1"));
	auto const gc = std::move(g);
	CHECK(gc.connections({1, 1}) == std::vector<std::pair<int, int>>{{2, 2}});
}