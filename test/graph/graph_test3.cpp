#include "gdwg/graph.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

// Collection 3 - Complex Iterators
TEST_CASE("begin == end if there are no nodes") {
	auto g = gdwg::graph<int, std::set<int>>{1, 2, 3};
	CHECK(g.begin() == g.end());
}
TEST_CASE("begin == end if there are nodes but no connections") {
	auto g = gdwg::graph<int, std::set<int>>{1, 2, 3};
	CHECK(g.begin() == g.end());
}
TEST_CASE("decrement from end provides last value whether or not connection is at the last node") {
	auto g = gdwg::graph<std::string, int>{"A", "B", "C", "D"};
	CHECK(g.insert_edge("A", "B", 1));
	CHECK(g.insert_edge("A", "C", 2));
	auto itr = g.end();
	--itr;
	CHECK((*itr).from == "A");
	CHECK((*itr).to == "C");
	CHECK((*itr).weight == 2);

	CHECK(g.insert_edge("A", "D", 3));
	auto const gc = std::move(g);
	auto itrc = gc.end();
	--itrc;
	CHECK((*itrc).from == "A");
	CHECK((*itrc).to == "D");
	CHECK((*itrc).weight == 3);
}

TEST_CASE("increment operator can skip nodes with no edge and also works with nodes with multiple "
          "weights") {
	auto g = gdwg::graph<int, int>{1, 2, 3, 4};
	CHECK(g.insert_edge(1, 2, 1));
	CHECK(g.insert_edge(3, 1, 2));
	CHECK(g.insert_edge(3, 1, 6));
	auto itr = g.begin();
	++itr;
	CHECK((*itr).from == 3);
	CHECK((*itr).to == 1);
	CHECK((*itr).weight == 2);

	auto const gc = std::move(g);
	auto itrc = gc.find(1, 2, 1);
	++itrc;
	++itrc;
	CHECK((*itrc).from == 3);
	CHECK((*itrc).to == 1);
	CHECK((*itrc).weight == 6);
	CHECK(++itrc == gc.end());
}
TEST_CASE("decrement operator can skip nodes with no edge and also works with nodes with multiple "
          "weights") {
	auto g = gdwg::graph<int, int>{1, 2, 3};
	CHECK(g.insert_edge(1, 2, 1));
	CHECK(g.insert_edge(3, 1, 2));
	CHECK(g.insert_edge(3, 1, 6));
	auto itr = g.end();
	--itr;
	CHECK((*itr).from == 3);
	CHECK((*itr).to == 1);
	CHECK((*itr).weight == 6);

	auto const gc = std::move(g);
	auto itrc = gc.find(3, 1, 6);
	--itrc;
	--itrc;
	CHECK((*itrc).from == 1);
	CHECK((*itrc).to == 2);
	CHECK((*itrc).weight == 1);
	CHECK(--itrc == gc.begin());
}
TEST_CASE("== on iterators returns false if they are different graphs") {
	auto g = gdwg::graph<int, std::string>{1, 2, 3};
	auto g2 = g;
	CHECK(not(g.begin() == g2.begin()));
	CHECK(not(g.end() == g2.end()));

	g.insert_edge(2, 2, "1");
	auto const gc = g;
	auto const gc2 = g;
	CHECK(not(gc.end() == gc2.end()));
}
TEST_CASE("== on iterators returns false if they are different empty graphs") {
	auto g = gdwg::graph<int, std::string>{};
	auto g2 = g; // NOLINT(performance-unnecessary-copy-initialization)
	CHECK(not(g.begin() == g2.begin()));
	CHECK(not(g.end() == g2.end()));
}
