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

### Modifiers
`auto insert_node(N const& value) -> bool;`

Insert a node if the node does not already exist. Returns true on success.


`auto insert_edge(N const& src, N const& dst, E const& weight) -> bool;`

Inserts an edge if it doesnt already exists. Returns true on success.


`auto merge_replace_node(N const& old_data, N const& new_data) -> void;`

The node equivalent to `old_data` in the graph are replaced with instances of `new_data`. After completing, every incoming and outgoing edge of old_data becomes an incoming/ougoing edge of new_data, except that duplicate edges shall be removed. Both `old_date` and `new_data` must be existing nodes.


`auto erase_node(N const& value) -> bool;`

Erases a node. Returns true on success.


`auto erase_edge(N const& src, N const& dst, E const& weight) -> bool;`

`auto erase_edge(iterator i) -> iterator;`

`auto erase_edge(iterator i, iterator s) -> iterator;`


Erase edges.


`auto clear() noexcept -> void;`

Erases all nodes.

### Accessors

`[[nodiscard]] auto is_node(N const& value) -> bool;`

Returns true if node exists in graph


`[[nodiscard]] auto empty() -> bool;`

Returns true if graph has no nodes


`[[nodiscard]] auto is_connected(N const& src, N const& dst) -> bool;`

Returns true if src is connected to dst


`[[nodiscard]] auto nodes() -> std::vector<N>;`

Returns vector of nodes


`[[nodiscard]] auto weights(N const& src, N const& dst) -> std::vector<E>;`

Returns list of weights between two nodes


`[[nodiscard]] auto find(N const& src, N const& dst, E const& weight) -> iterator;`

Returns iterator to an edge, if it exists.


`[[nodiscard]] auto connections(N const& src) -> std::vector<N>;`

Returns list of nodes connected from src.


### Iterator access

`[[nodiscard]] auto begin() const -> iterator;`

Iterator to the first element.


`[[nodiscard]] auto end() const -> iterator;`

Iterator to beyond the last element.


### Comparisons 

`[[nodiscard]] auto operator==(graph const& other) -> bool;`

Returns true if all nodes and edges are equal
