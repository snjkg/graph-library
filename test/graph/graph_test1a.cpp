//////////////////////////////////////////////////////////////////////////
// *** COMP6771 Assignment 2  ***
//
// Test Rationale and Approach
//
// The apporach is to build up the tests from a collection of base tests
// to more specialised tests, and for each colection, ensure there is coverage
// by testing all functionality relavant to that collection.
//
// The tests cases can be classed into four collections.
//
// Collection 1 - Base Tests - graph_test1[a-e].cpp:  start with trivial
// test cases to check that each piece of functionality is working correctly
// in isolation. This reason for this is that it will enable a fundamental error
// with a function be immediately identified, and there no need to consider more
// difficult test cases in the debugging. Each of the base tests will test the
// preconditions and post conditions according to the specification
// 1a - Constructors
// 1b - Modifiers
// 1c - Accessors
// 1d - Iterator
// 1e - Sundry
//
// Collection 2 - Error Handling - graph_test2.cpp: scenarios
// that are known to raise an exception are tested. The reason we test for this
// is that error handling is defined as a requirement for specific funcionality,
// so to ensure we have full coverage and not miss some error handling requirement,
// it is best organised in a collection.
//
// Collection 3 - Iterators - graph_test4.cpp: I consider the iterators implementation
// to be of high complexity, in particular it involved various condierations related
// to abstracting three iterators into a single iterator, including when some underlying
// collections are empty, and the requirement to move in both directions. Here, we
// test these more complex scenarios.
//
// Coverage
// To ensure we have full coverage, each declared constructor, destructor,
// method, operator, function is used at least once in Collection 1.
// In Collection 2 we test every item with a defined error handling
// in the requirements. In colection 3 we take a similar approach to Collection 1
// except.
// To check const correctness, a selection of both const and non-const
// test cases are performed.
// To check template parameters, a selection of primative and complex
// types are used.
//
//////////////////////////////////////////////////////////////////////////
#include "gdwg/graph.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <set>
#include <string>

// Collection 1a - Constructors
TEST_CASE("construction using default constructor initialises empty graph") {
	auto g = gdwg::graph<std::set<int>, std::string>{};
	CHECK(g.empty());

	auto const gc = gdwg::graph<double, std::pair<int, int>>{};
	CHECK(gc.empty());
}
TEST_CASE("construction using initialiser list initialises graph with same values") {
	auto g = gdwg::graph<std::string, int>{"z", "y"};
	CHECK(g.is_node("z"));
	CHECK(g.is_node("y"));

	auto const gc = gdwg::graph<int, std::string>{3, 1, 2};
	CHECK(gc.is_node(1));
	CHECK(gc.is_node(2));
	CHECK(gc.is_node(3));
}
TEST_CASE("construction using initialiser list does not insert duplicate values") {
	auto g =
	   gdwg::graph<std::string, std::string>{std::string("a"), std::string("a"), std::string("c")};
	CHECK(g.nodes() == std::vector<std::string>{"a", "c"});

	auto const gc = gdwg::graph<double, std::string>{1.0, 1.1, 1.0};
	CHECK(gc.nodes() == std::vector<double>{1.0, 1.1});
}
TEST_CASE("construction using iterator initialises graph with same values ") {
	auto v = std::vector<std::string>{"ab", "a", "abc"};
	auto g = gdwg::graph<std::string, int>{v.begin(), v.end()};
	CHECK(g.is_node("a"));
	CHECK(g.is_node("ab"));
	CHECK(g.is_node("abc"));

	auto const vc = std::vector<int>{3, 4};
	auto const gc = gdwg::graph<int, int>{vc.begin(), vc.end()};
	CHECK(gc.is_node(3));
	CHECK(gc.is_node(4));
	CHECK(gc.is_node(3));
}
TEST_CASE("construction using copy constructor initialises graph with same values") {
	// This will not compile straight away
	auto g1 = gdwg::graph<std::string, std::string>{"p", "s", "a"};
	CHECK(g1.insert_edge("a", "s", "10km"));
	auto g2 = gdwg::graph<std::string, std::string>{g1};
	CHECK(g1.is_connected("a", "s"));
	CHECK(g2.is_node("p"));
	CHECK(g2.is_node("s"));
	CHECK(g2.is_node("s"));
	CHECK(g2.is_connected("a", "s"));
	CHECK(g1 == g2);
	CHECK(g1.erase_node("p"));
	auto const g3 = gdwg::graph<std::string, std::string>{g1};
	CHECK(g1 != g2);
	CHECK(g1 == g3);
}
TEST_CASE("construction using move constructor initialises graph with same values") {
	auto g1 = gdwg::graph<std::string, std::string>{"p", "s", "a"};
	CHECK(g1.insert_edge("a", "s", "10km"));
	auto g2 = gdwg::graph<std::string, std::string>{std::move(g1)};
	CHECK(g2.is_node("p"));
	CHECK(g2.is_node("s"));
	CHECK(g2.is_node("s"));
	CHECK(g2.is_connected("a", "s"));
	auto const g3 = gdwg::graph<std::string, std::string>{std::move(g2)};
	CHECK(g3.is_node("p"));
	CHECK(g3.is_node("s"));
	CHECK(g3.is_node("s"));
	CHECK(g3.is_connected("a", "s"));
}
TEST_CASE("construction using assignment operator initialises graph that is independant") {
	auto g1 = gdwg::graph<std::string, std::string>{"p", "s", "a"};
	CHECK(g1.insert_edge("a", "s", "10km"));
	auto g2 = gdwg::graph<std::string, std::string>{};
	g2 = g1;
	CHECK(g1 == g2);
	CHECK(g1.replace_node("s", "t"));
	CHECK(g1.is_node("t"));
	CHECK(not g2.is_node("t"));
}