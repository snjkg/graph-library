#ifndef GDWG_GRAPH_HPP
#define GDWG_GRAPH_HPP

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>
namespace gdwg {
	template<typename N, typename E>
	class graph {
	private:
		template<typename V>
		struct value_comparator;

		template<typename R>
		using optional_ref = std::optional<std::reference_wrapper<R>>;

		using node = std::unique_ptr<N>;
		using node_observer = N*;
		using edge = std::unique_ptr<E>;
		using weights_set = std::set<edge, value_comparator<E>>;
		using destinations_map = std::map<node_observer, weights_set, value_comparator<N>>;
		using nodes_map = std::map<node, destinations_map, value_comparator<N>>;

	public:
		class iterator;

		struct value_type {
			N from;
			N to;
			E weight;
		};

		// Constructors
		graph() = default;

		graph(std::initializer_list<N> il)
		: graph(il.begin(), il.end()) {}

		template<typename InputIt>
		graph(InputIt first, InputIt last) {
			std::for_each(first, last, [this](auto const& value) { this->insert_node(value); });
		}

		graph(graph&& other) noexcept
		: nodes_{std::exchange(other.nodes_, nodes_map{})} {}

		auto operator=(graph&& other) noexcept -> graph& {
			if (other == this) {
				this->clear();
			}
			else {
				auto g = graph(other);
				std::swap(g.nodes_, this->nodes_);
			}
			return *this;
		}

		graph(graph const& other) {
			for (auto const& [src, destinations] : other.nodes_) {
				this->insert_node(*src);
			}

			for (auto const& [src, destinations] : other.nodes_) {
				for (auto const& [dst, weights] : destinations) {
					for (auto const& w : weights) {
						this->insert_edge(*src, *dst, *w);
					}
				}
			}
		}

		auto operator=(graph const& other) -> graph& {
			auto g = graph(other);
			std::swap(g.nodes_, this->nodes_);
			return *this;
		}

		// Modifiers

		auto insert_node(N const& value) -> bool {
			auto [itr, b] =
			   this->nodes_.emplace(std::make_pair(std::make_unique<N>(value), destinations_map{}));
			;
			return b;
		}

		auto insert_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (not(this->is_node(src) and this->is_node(dst))) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::insert_edge when "
				                         "either src or "
				                         "dst "
				                         "node does not exist");
			}

			auto& weights = this->get_weights_for_insertion(this->get_destinations(src), dst);
			if (weights.count(weight) == 0U) {
				return weights.emplace(std::make_unique<E>(weight)).second;
			}
			return false;
		}

		auto replace_node(N const& old_data, N const& new_data) -> bool {
			if (not this->is_node(old_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::replace_node on a "
				                         "node that "
				                         "doesn't exist");
			}
			if (this->is_node(new_data)) {
				return false;
			}

			auto old_node = this->nodes_.extract(this->nodes_.find(old_data));
			*(old_node.key()) = new_data;
			this->nodes_.insert(std::move(old_node));
			return true;
		}

		auto merge_replace_node(N const& old_data, N const& new_data) -> void {
			if (not(this->is_node(old_data) and this->is_node(new_data))) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node "
				                         "on old or new "
				                         "data if they don't exist in the graph");
			}
			if (old_data == new_data) {
				return;
			}
			auto& new_node_destinations = this->get_destinations(new_data);
			auto old_node = this->nodes_.extract(this->nodes_.find(old_data));

			// Merge destinations from old node to be from new node
			for (auto& [dst, weights] : old_node.mapped()) {
				this->get_weights_for_insertion(new_node_destinations, *dst).merge(weights);
			}

			// Merge destinations to old node to be to new node
			for (auto& [src, destinations] : this->nodes_) {
				auto weights = this->get_weights(destinations, old_data);
				if (weights.has_value()) {
					this->get_weights_for_insertion(destinations, new_data).merge(weights->get());
					destinations.erase(destinations.find(old_data));
				}
			}
		}

		auto erase_node(N const& value) noexcept -> bool {
			if (not this->is_node(value)) {
				return false;
			}
			auto itr = this->nodes_.find(value);
			for (auto& [src, destinations] : this->nodes_) {
				destinations.erase(itr->first.get());
			}
			this->nodes_.erase(itr);
			return true;
		}

		auto erase_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (not(this->is_node(src) and this->is_node(dst))) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or "
				                         "dst if they don't exist in the graph");
			}
			auto& destinations = this->get_destinations(src);
			auto weights = this->get_weights(destinations, dst);
			if (not weights.has_value() or weights->get().count(weight) == 0U) {
				return false;
			}
			weights->get().erase(weights->get().find(weight));
			if (weights->get().size() == 0U) {
				destinations.erase(destinations.find(dst));
			}
			return true;
		}
		auto erase_edge(iterator i) -> iterator {
			auto next = std::next(i);

			// retrieve non const iterator from const iterator in constant time
			// https://stackoverflow.com/questions/765148/how-to-remove-constness-of-const-iterator
			auto& destinations = this->nodes_.erase(i.n_iter_, i.n_iter_)->second;
			auto& weights = destinations.erase(i.d_iter_, i.d_iter_)->second;

			if (weights.erase(i.w_iter_) == weights.end()) {
				if (next.w_iter_ == weights.cend()) {
					next.w_iter_ = typename iterator::w_iter{};
				}
				destinations.erase(i.d_iter_);
			}

			return next;
		}
		auto erase_edge(iterator i, iterator s) -> iterator {
			for (; i != s; i = erase_edge(i)) {
			}
			return i;
		}

		auto clear() noexcept -> void {
			this->nodes_.clear();
		}

		[[nodiscard]] auto is_node(N const& value) const noexcept -> bool {
			return this->nodes_.count(value) != 0U;
		}

		[[nodiscard]] auto empty() const noexcept -> bool {
			return this->nodes_.empty();
		}

		[[nodiscard]] auto is_connected(N const& src, N const& dst) const -> bool {
			if (not(this->is_node(src) and this->is_node(dst))) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src "
				                         "or dst node don't exist in the graph");
			}
			auto const& destinations = this->get_destinations(src);
			return destinations.count(dst) != 0U;
		}

		[[nodiscard]] auto nodes() const -> std::vector<N> {
			auto vec = std::vector<N>(this->nodes_.size());
			std::transform(this->nodes_.cbegin(), this->nodes_.cend(), vec.begin(), [](auto const& src) {
				return *(src.first);
			});
			return vec;
		}
		[[nodiscard]] auto weights(N const& src, N const& dst) const -> std::vector<E> {
			if (not(this->is_node(src) and this->is_node(dst))) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::weights if src or "
				                         "dst node don't exist in the graph");
			}
			auto const& destinations = this->get_destinations(src);
			auto weights_result = this->get_weights(destinations, dst);
			if (not weights_result.has_value()) {
				return std::vector<E>{};
			}
			auto vec = std::vector<E>(weights_result->get().size());
			std::transform(weights_result->get().cbegin(),
			               weights_result->get().cend(),
			               vec.begin(),
			               [](auto const& wt) { return *wt; });
			return vec;
		}
		[[nodiscard]] auto find(N const& src, N const& dst, E const& weight) const noexcept
		   -> iterator {
			if (this->is_node(src) and this->is_node(dst)) {
				auto& destinations = this->get_destinations(src);
				auto weights = this->get_weights(destinations, dst);
				if (weights.has_value() and weights->get().count(weight) != 0U) {
					return iterator{nodes_.find(src),
					                destinations.find(dst),
					                weights->get().find(weight),
					                this->nodes_.cbegin(),
					                this->nodes_.cend()};
				}
			}
			return this->end();
		}
		[[nodiscard]] auto connections(N const& src) const -> std::vector<N> {
			if (not this->is_node(src)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src "
				                         "doesn't exist in the graph");
			}
			auto const& destinations = this->get_destinations(src);
			auto vec = std::vector<N>(destinations.size());
			std::transform(destinations.cbegin(), destinations.cend(), vec.begin(), [](auto const& dst) {
				return *(dst.first);
			});
			return vec;
		}
		auto begin() const noexcept -> iterator {
			if (this->nodes_.size() == 0U) {
				return iterator{this->nodes_.cend(),
				                typename iterator::d_iter{},
				                typename iterator::w_iter{},
				                this->nodes_.cbegin(),
				                this->nodes_.cend()};
			}
			auto first = std::find_if(this->nodes_.cbegin(), this->nodes_.cend(), [](auto const& src) {
				return not(src.second).empty();
			});
			if (first == this->nodes_.end()) {
				return iterator{this->nodes_.cend(),
				                std::prev(this->nodes_.cend())->second.cend(),
				                typename iterator::w_iter{},
				                this->nodes_.cbegin(),
				                this->nodes_.cend()};
			}

			return iterator{first,
			                first->second.cbegin(),
			                first->second.cbegin()->second.cbegin(),
			                this->nodes_.cbegin(),
			                this->nodes_.cend()};
		};
		auto end() const noexcept -> iterator {
			if (this->nodes_.size() == 0U) {
				return iterator{this->nodes_.cend(),
				                typename iterator::d_iter{},
				                typename iterator::w_iter{},
				                this->nodes_.cbegin(),
				                this->nodes_.cend()};
			}
			return iterator{this->nodes_.cend(),
			                std::prev(this->nodes_.cend())->second.cend(),
			                typename iterator::w_iter{},
			                this->nodes_.cbegin(),
			                this->nodes_.cend()};
		};
		[[nodiscard]] auto operator==(graph const& other) const noexcept -> bool {
			auto weight_pred = [](auto const& lw, auto const& rw) { return *lw == *rw; };
			auto dst_pred = [weight_pred](auto const& le, auto const& re) {
				return *(le.first) == *(re.first)
				       and std::equal(le.second.cbegin(),
				                      le.second.cend(),
				                      re.second.cbegin(),
				                      re.second.cend(),
				                      weight_pred);
			};
			auto node_pred = [dst_pred](auto const& ln, auto const& rn) {
				return *(ln.first) == *(rn.first)
				       and std::equal(ln.second.cbegin(),
				                      ln.second.cend(),
				                      rn.second.cbegin(),
				                      rn.second.cend(),
				                      dst_pred);
			};
			return std::equal(this->nodes_.cbegin(),
			                  this->nodes_.cend(),
			                  other.nodes_.cbegin(),
			                  other.nodes_.cend(),
			                  node_pred);
		}
		friend auto operator<<(std::ostream& os, graph const& g) -> std::ostream& {
			for (auto const& [src, edges] : g.nodes_) {
				os << *src << " (" << std::endl;
				for (auto const& [dst, weights] : edges) {
					for (auto const& w : weights) {
						os << "  " << *dst << " | " << *w << std::endl;
					}
				}
				os << ")" << std::endl;
			}
			return os;
		}

	private:
		template<typename V>
		struct value_comparator {
			using is_transparent = void;
			auto operator()(const std::unique_ptr<V>& lhs, const std::unique_ptr<V>& rhs) const -> bool {
				return *lhs < *rhs;
			}
			auto operator()(const std::unique_ptr<V>& lhs, const V& rhs) const -> bool {
				return *lhs < rhs;
			}
			auto operator()(const V& lhs, const std::unique_ptr<V>& rhs) const -> bool {
				return lhs < *rhs;
			}
			auto operator()(const V* lhs, const V* rhs) const -> bool {
				return *lhs < *rhs;
			}
			auto operator()(const V* lhs, const V& rhs) const -> bool {
				return *lhs < rhs;
			}
			auto operator()(const V& lhs, const V* rhs) const -> bool {
				return lhs < *rhs;
			}
		};

		[[nodiscard]] auto get_destinations(N const& src) -> destinations_map& {
			auto itr = this->nodes_.find(src);
			if (itr == this->nodes_.end()) {
				throw std::out_of_range{"Source does not exist"};
			}
			return itr->second;
		}
		[[nodiscard]] auto get_destinations(N const& src) const -> destinations_map const& {
			auto itr = this->nodes_.find(src);
			if (itr == this->nodes_.end()) {
				throw std::out_of_range{"Source does not exist"};
			}
			return itr->second;
		}
		auto get_weights_for_insertion(destinations_map& em, N const& dst) -> weights_set& {
			auto itr = em.find(dst);
			if (itr == em.end()) {
				std::tie(itr, std::ignore) =
				   em.emplace(std::pair{this->nodes_.find(dst)->first.get(), weights_set{}});
			}
			return itr->second;
		}
		[[nodiscard]] auto get_weights(destinations_map const& em, N const& dst) const noexcept
		   -> optional_ref<weights_set const> {
			auto itr = em.find(dst);
			if (itr == em.end()) {
				return std::nullopt;
			}
			return optional_ref<weights_set const>{std::cref(itr->second)};
		}
		[[nodiscard]] auto get_weights(destinations_map& em, N const& dst) noexcept
		   -> optional_ref<weights_set> {
			auto itr = em.find(dst);
			if (itr == em.end()) {
				return std::nullopt;
			}
			return optional_ref<weights_set>{std::ref(itr->second)};
		}

		nodes_map nodes_{};

	public:
		class iterator {
		private:
			using n_iter = typename nodes_map::const_iterator;
			using d_iter = typename destinations_map::const_iterator;
			using w_iter = typename weights_set::const_iterator;

		public:
			using value_type = graph<N, E>::value_type;
			using reference = value_type;
			using pointer = void;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::bidirectional_iterator_tag;

			// Iterator constructor
			iterator() = default;

			// Iterator source
			auto operator*() -> reference {
				return reference{*(n_iter_->first), *(d_iter_->first), **w_iter_};
			};
			// auto operator->() -> pointer not required

			// Iterator traversal
			auto operator++() -> iterator& {
				for (++w_iter_; w_iter_ == d_iter_->second.end(); w_iter_ = d_iter_->second.begin()) {
					w_iter_ = w_iter{};
					for (++d_iter_; d_iter_ == n_iter_->second.end(); d_iter_ = n_iter_->second.begin()) {
						if (++n_iter_ == n_iter_end_) {
							return *this;
						}
					}
				}
				return *this;
			}
			auto operator++(int) -> iterator {
				auto ret = iterator{*this};
				++*this;
				return ret;
			}
			auto operator--() -> iterator& {
				if (n_iter_ == n_iter_end_) {
					for (auto n_new = std::make_reverse_iterator(n_iter_end_);
					     n_new != std::make_reverse_iterator(n_iter_begin_);
					     ++n_new)
					{
						if (not n_new->second.empty()) {
							n_iter_ = std::prev(n_new.base());
							d_iter_ = std::prev(n_iter_->second.end());
							w_iter_ = std::prev(d_iter_->second.end());
							return *this;
						}
					}
					return *this;
				}

				for (; w_iter_ == d_iter_->second.begin(); w_iter_ = (--d_iter_)->second.end()) {
					for (; d_iter_ == n_iter_->second.begin(); d_iter_ = (--n_iter_)->second.end()) {
						if (n_iter_ == n_iter_begin_) {
							return *this;
						}
					}
				}
				--w_iter_;

				return *this;
			}
			auto operator--(int) -> iterator {
				auto ret = iterator{*this};
				--*this;
				return ret;
			}

			// Iterator comparison
			auto operator==(iterator const& other) const noexcept -> bool {
				return this->n_iter_ == other.n_iter_ and this->d_iter_ == other.d_iter_
				       and this->w_iter_ == other.w_iter_ and this->n_iter_begin_ == other.n_iter_begin_
				       and this->n_iter_end_ == other.n_iter_end_;
			}

		private:
			n_iter n_iter_;
			d_iter d_iter_;
			w_iter w_iter_;
			n_iter n_iter_begin_;
			n_iter n_iter_end_;

			iterator(n_iter n, d_iter e, w_iter w, n_iter nb, n_iter ne) noexcept
			: n_iter_{n}
			, d_iter_{e}
			, w_iter_{w}
			, n_iter_begin_{nb}
			, n_iter_end_{ne} {};

			friend auto gdwg::graph<N, E>::erase_edge(iterator i) -> iterator;
			friend auto
			gdwg::graph<N, E>::find(N const& src, N const& dst, E const& weight) const noexcept
			   -> iterator;
			friend auto gdwg::graph<N, E>::begin() const noexcept -> iterator;
			friend auto gdwg::graph<N, E>::end() const noexcept -> iterator;
		};
	};

} // namespace gdwg

#endif // GDWG_GRAPH_HPP
