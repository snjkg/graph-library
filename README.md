# graph-library
## Background
This is a generic directed weighted graph (GDWG) with value-semantics in C++. 
Both the data stored at a node and the weight stored at an edge are be parameterised types. 

The types may be different. Example:
`using graph = gdwg::graph<std::string, int>;`


Formally, this directed weighted graph G = (N, E) will consist of a set of nodes N and a set of weighted edges E.
All nodes are unique, that is to say, no two nodes will have the same value and shall not compare equal using operator==.
Given a node, an edge directed into it is called an incoming edge and an edge directed out of it is called an outgoing edge. The in-degree of a node is the number of its incoming edges. Similarly, the out-degree of a node is the number of its outgoing edges. Given a directed edge from src to dst, src is the source node and dst is known as the destination node.
Edges can be reflexive, that is to say, the source and destination nodes of an edge could be the same.
G is a multi-edged graph, as there may be two edges from the same source node to the same destination node with two different weights. Two edges from the same source node to the same destination node cannot have the same weight.

## Usage
Include `include/gdwg/graph.hpp`

### Constructors
`graph();`
Value initialises all members.

`graph(std::initializer_list<N> il);`
Initialises graph with nodes in list

`template<typename InputIt>
graph(InputIt first, InputIt last);`
Initialises the graph’s node collection with the range [first, last)

`graph(graph&& other) noexcept;`
Move contructor

`auto operator=(graph&& other) noexcept -> graph&;`
Move assignment

`graph(graph const& other);`
Copy contructor

`auto operator=(graph const& other) -> graph&;`
Copy assignment

