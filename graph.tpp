#include <iterator>
#include <iostream>
#include <vector>

#include "assignments/dg/graph.h"

template <typename N, typename E>
gdwg::Graph<N, E>::Graph() : nodes_{std::set<std::shared_ptr<Node>, CompareByValue<Node>>{}} {}

template <typename N, typename E>
gdwg::Graph<N,E>::Graph(typename std::vector<N>::const_iterator begin,
                        typename std::vector<N>::const_iterator end)
    : nodes_{std::set<std::shared_ptr<Node>, CompareByValue<Node>>{}} {
  int len = end - begin;
  for (int i = 0; i < len; ++i) {
    nodes_.emplace(std::make_shared<Node>(*begin++));
  }
}

template <typename N, typename E>
gdwg::Graph<N,E>::Graph(typename std::vector<std::tuple<N,N,E>>::const_iterator begin,
                        typename std::vector<std::tuple<N,N,E>>::const_iterator end)
    : nodes_{std::set<std::shared_ptr<Node>, CompareByValue<Node>>{}}{
  int len = end - begin;
  for (int i = 0; i < len; ++i) {
    auto [src, dest, cost] = *begin++;
  }

}

template <typename N, typename E>
gdwg::Graph<N,E>::Graph(std::initializer_list<N> list)
    : nodes_{std::set<std::shared_ptr<Node>, CompareByValue<Node>>{}} {
  for (const auto& n : list) {
    auto ptr = std::make_shared<Node>(n);
    nodes_.emplace(ptr);
  }
}

template <typename N, typename E>
gdwg::Graph<N,E>::Graph(const gdwg::Graph<N,E>& g)
    : nodes_{std::set<std::shared_ptr<Node>, CompareByValue<Node>>{}}{
  g;
}

template <typename N, typename E>
gdwg::Graph<N,E>::Graph(gdwg::Graph<N,E>&& g) noexcept
    : nodes_{std::move(g.nodes_)} {}

template<typename N, typename E>
bool gdwg::Graph<N, E>::InsertNode(const N& val) {
  auto new_node = std::make_shared<Node>(val);
  auto inserted = this->nodes_.emplace(new_node);
  return inserted.second;
}

template<typename N, typename E>
gdwg::Graph<N, E>::Node::Node(N value) : value_{value} {

}

template<typename N, typename E>
N gdwg::Graph<N, E>::Node::GetValue() const {
  return this->value_;
}

template<typename N, typename E>
void gdwg::Graph<N, E>::Node::AddEdgeTo(std::shared_ptr<Node>& n, E cost) {
  std::weak_ptr<Node> edgeTo = n;
  this->edges_to_.emplace(edgeTo, cost);
}



