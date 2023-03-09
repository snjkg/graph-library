#include "gdwg/graph.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

// Collection 1d - Iterators
TEST_CASE("begin points to first connection") {
	auto g = gdwg::graph<int, std::set<int>>{1, 2, 3};
	CHECK(g.insert_edge(2, 2, std::set<int>{4}));
	CHECK(g.insert_edge(2, 2, std::set<int>{5}));
	CHECK(g.insert_edge(2, 3, std::set<int>{6}));
	auto itr = g.begin();
	CHECK((*itr).from == 2);
	CHECK((*itr).to == 2);
	CHECK((*itr).weight == std::set<int>{4});

	auto const gc = std::move(g);
	auto itrc = gc.begin();
	CHECK((*itrc).from == 2);
	CHECK((*itrc).to == 2);
	CHECK((*itrc).weight == std::set<int>{4});
}
TEST_CASE("trivially increment and decrement iterator (same dst,src nodes)") {
	auto g = gdwg::graph<int, int>{1};
	CHECK(g.insert_edge(1, 1, 1));
	CHECK(g.insert_edge(1, 1, 2));
	auto itr = g.begin();
	++itr;
	CHECK((*itr).from == 1);
	CHECK((*itr).to == 1);
	CHECK((*itr).weight == 2);
	--itr;
	CHECK((*itr).from == 1);
	CHECK((*itr).to == 1);
	CHECK((*itr).weight == 1);
}

TEST_CASE("increment from last value reaches end") {
	auto g = gdwg::graph<int, std::set<int>>{1, 2, 3};
	CHECK(g.insert_edge(2, 2, std::set<int>{4}));
	CHECK(g.insert_edge(2, 2, std::set<int>{5}));
	auto itr = g.begin();
	++itr;
	CHECK((*itr).from == 2);
	CHECK((*itr).to == 2);
	CHECK((*itr).weight == std::set<int>{5});
	++itr;
	CHECK(itr == g.end());
}
TEST_CASE("decrement from end reaches last value") {
	auto g = gdwg::graph<int, std::set<int>>{1, 2, 3};
	CHECK(g.insert_edge(3, 2, std::set<int>{4}));
	CHECK(g.insert_edge(3, 2, std::set<int>{8}));
	auto itr = g.end();
	--itr;
	CHECK((*itr).from == 3);
	CHECK((*itr).to == 2);
	CHECK((*itr).weight == std::set<int>{8});
}

TEST_CASE("postfix increment and decrement returns original value") {
	auto g = gdwg::graph<std::string, std::string>{"A", "B", "C", "D"};
	CHECK(g.insert_edge("A", "B", "1"));
	CHECK(g.insert_edge("A", "C", "2"));
	CHECK(g.insert_edge("A", "D", "3"));
	CHECK(g.insert_edge("B", "B", "1"));

	CHECK(g.begin() == (g.begin())++);
	auto const gc = std::move(g);
	CHECK(gc.begin() == (gc.begin())++);
}
TEST_CASE("iterator obtained from the same method returns true for == operator") {
	auto g = gdwg::graph<std::string, std::string>{"A", "B", "C", "D"};
	CHECK(g.insert_edge("A", "B", "1"));
	CHECK(g.insert_edge("A", "C", "2"));
	CHECK(g.insert_edge("A", "D", "3"));
	CHECK(g.insert_edge("B", "B", "1"));

	CHECK(g.begin() == g.begin());
}
TEST_CASE("== on iterators are true even when they are obtained through increment/decrement and "
          "find") {
	auto g = gdwg::graph<int, std::string>{1, 2, 3};
	g.insert_edge(2, 2, "1");
	auto itr = g.find(2, 2, "1");
	CHECK(itr == g.begin());
	CHECK(--(++itr) == g.begin());

	g.insert_edge(2, 2, "2");
	g.insert_edge(2, 3, "3");
	auto const gc = g;
	auto itrc = g.find(2, 2, "2");
	CHECK(--itrc == g.begin());
	CHECK(++(++(itrc)) == g.find(2, 3, "3"));
}