#include "gdwg/graph.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

// Collection 1b - Modifiers
TEST_CASE("insert_node adds node but only if there is no equivelent node") {
	auto g = gdwg::graph<std::set<int>, int>{};
	CHECK(g.insert_node(std::set<int>{1, 2, 3}));
	CHECK(g.is_node(std::set<int>{1, 2, 3}));
	CHECK(not g.insert_node(std::set<int>{1, 2, 3}));
	CHECK(g.nodes() == std::vector<std::set<int>>{std::set<int>{1, 2, 3}});
}
TEST_CASE("insert_edge adds edge only if there is no equivelent edge of same weight") {
	auto g = gdwg::graph<int, std::set<int>>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, std::set<int>{4}));
	CHECK(not g.insert_edge(1, 2, std::set<int>{4}));
	CHECK(g.insert_edge(1, 2, std::set<int>{4, 5}));

	CHECK(g.weights(1, 2) == std::vector<std::set<int>>{std::set<int>{4}, std::set<int>{4, 5}});
}
TEST_CASE("replace_node replaces the node (and maintains edges) only if there is an equivelent "
          "node") {
	auto g = gdwg::graph<std::string, int>{"x", "y"};
	CHECK(g.insert_edge("x", "y", 4));
	CHECK(g.insert_edge("y", "x", 5));
	CHECK(g.insert_edge("y", "y", 6));
	CHECK(not g.replace_node("y", "x"));
	CHECK(g.replace_node("y", "z"));
	CHECK(not g.is_node("y"));
	CHECK(g.is_node("z"));
	CHECK(g.is_connected("x", "z"));
	CHECK(g.is_connected("z", "x"));
}
TEST_CASE("merge_replace_node merges the old node into the (existing) new node") {
	auto g = gdwg::graph<std::string, int>{"A", "B", "C", "D"};
	CHECK(g.insert_edge("A", "B", 1));
	CHECK(g.insert_edge("A", "C", 2));
	CHECK(g.insert_edge("A", "D", 3));
	g.merge_replace_node("A", "B");
	CHECK(not g.is_node("A"));
	CHECK(g.weights("B", "B") == std::vector<int>{1});
	CHECK(g.weights("B", "C") == std::vector<int>{2});
	CHECK(g.weights("B", "D") == std::vector<int>{3});
}
TEST_CASE("merge_replace_node merges the old node into the (existing) new node & deletes duplicate "
          "edge") {
	auto g = gdwg::graph<std::string, std::string>{"A", "B", "C", "D"};
	CHECK(g.insert_edge("A", "B", "1"));
	CHECK(g.insert_edge("A", "C", "2"));
	CHECK(g.insert_edge("A", "D", "3"));
	CHECK(g.insert_edge("B", "B", "1"));
	g.merge_replace_node("A", "B");
	CHECK(not g.is_node("A"));
	CHECK(g.weights("B", "B") == std::vector<std::string>{"1"});
	CHECK(g.weights("B", "C") == std::vector<std::string>{"2"});
	CHECK(g.weights("B", "D") == std::vector<std::string>{"3"});
}
TEST_CASE("erase_node erases node only if the node exists") {
	auto g = gdwg::graph<int, std::string>{1, 2, 3};
	CHECK(g.insert_edge(1, 1, "1"));
	CHECK(g.insert_edge(1, 2, "2"));
	CHECK(g.insert_edge(3, 1, "3"));
	CHECK(not g.erase_node(0));
	CHECK(g.erase_node(1));
	CHECK(not g.is_node(1));
	CHECK(g.is_node(2));
	CHECK(g.is_node(3));
}
TEST_CASE("erase_edge(src,dst,weight) erases edge only if the edge exists") {
	auto g = gdwg::graph<std::pair<int, int>, std::string>{{1, 1}, {2, 2}, {3, 3}};
	CHECK(g.insert_edge({1, 1}, {2, 2}, "1"));
	CHECK(g.insert_edge({2, 2}, {2, 2}, "2"));
	CHECK(not g.erase_edge({2, 2}, {1, 1}, "1"));
	CHECK(not g.erase_edge({1, 1}, {2, 2}, "2"));
	CHECK(g.erase_edge({1, 1}, {2, 2}, "1"));
	CHECK(g.connections({1, 1}).empty());
}
TEST_CASE("erase_edge(iterator) erases edge and returns iterator to next item") {
	auto g = gdwg::graph<int, int>{0, 1};
	CHECK(g.insert_edge(0, 1, 1));
	CHECK(g.insert_edge(0, 1, 2));
	CHECK(g.insert_edge(1, 1, 2));
	CHECK(g.erase_edge(g.begin()) == g.find(0, 1, 2));
	CHECK(g.weights(0, 1) == std::vector<int>{2});
	CHECK(g.erase_edge(g.find(0, 1, 2)) == g.find(1, 1, 2));
	CHECK(g.erase_edge(g.find(1, 1, 2)) == g.begin());
	CHECK(g.connections(0).empty());
}
TEST_CASE("erase_edge(iterator,iterator) erases edge and returns iterator to next item") {
	auto g = gdwg::graph<int, int>{-1, 0, 1, 2};
	CHECK(g.insert_edge(0, 1, 1));
	CHECK(g.insert_edge(0, 1, 2));
	CHECK(g.erase_edge(g.begin(), g.end()) == g.begin());
	CHECK(g.connections(0).empty());
	CHECK(g.insert_edge(0, 1, 2));
	CHECK(g.insert_edge(1, 2, 2));
	CHECK(g.insert_edge(1, 2, 1));
	CHECK(g.erase_edge(g.find(1, 2, 1), g.find(1, 2, 2)) == g.find(1, 2, 2));
	CHECK(g.is_connected(0, 1));
	CHECK(g.weights(1, 2) == std::vector<int>{2});
}
TEST_CASE("erase_edge(iterator,iterator) erases graph") {
	auto g = gdwg::graph<std::string, int>{"a", "b", "c"};
	CHECK(g.insert_edge("a", "b", 1));
	g.clear();
	CHECK(g.empty());
}
