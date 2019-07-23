#include <iterator>
#include <iostream>
#include <vector>
#include <algorithm>
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
bool gdwg::Graph<N,E>::InsertNode(const N& val) {
  auto new_node = std::make_shared<Node>(val);
  auto inserted = this->nodes_.emplace(new_node);
  return inserted.second;
}

template <typename N, typename E>
bool gdwg::Graph<N,E>::InsertEdge(const N& src, const N& dest, const E& w) {
  if (!IsNode(src) || !IsNode(dest)) {
    throw std::runtime_error("Cannot call Graph::InsertEdge when either src or dst node does not exist");
  }

  auto src_it = std::find_if(nodes_.begin(), nodes_.end(), [&src](std::shared_ptr<Node> const& n) {return n->GetValue() == src;});
  auto dst_it = std::find_if(nodes_.begin(), nodes_.end(), [&dest](std::shared_ptr<Node> const& n) {return n->GetValue() == dest;});

  return src_it->get()->AddEdgeTo(*dst_it, w);
}


template <typename N, typename E>
bool gdwg::Graph<N,E>::IsNode(const N& val) {
  for (const auto& n : nodes_) {
    if (n->GetValue() == val) {
      return true;
    }
  }
  return false;
}

template <typename N, typename E>
bool gdwg::Graph<N,E>::IsConnected(const N& src, const N& dest) {
  if (!IsNode(src) || !IsNode(dest)) {
    throw std::runtime_error("Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
  }
  auto src_it = std::find_if(nodes_.begin(), nodes_.end(), [&src](std::shared_ptr<Node> const& n) {return n->GetValue() == src;});

  return src_it->get()->IsEdge(dest);
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N,E>::GetNodes() {
  std::vector<N> v;
  for (const auto& n : this->nodes_) {
    v.push_back(n->GetValue());
  }
  std::sort(v.begin(), v.end());
  return v;
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N,E>::GetConnected(const N& src) {
  if (!IsNode(src)) {
    throw std::out_of_range("Cannot call Graph::GetConnected if src doesn't exist in the graph");
  }
  std::vector<N> v;

  auto src_it = std::find_if(nodes_.begin(), nodes_.end(), [&src](std::shared_ptr<Node> const& n) {return n->GetValue() == src;});

  return src_it->get()->GetEdges();
}

template <typename N, typename E>
std::vector<E> gdwg::Graph<N,E>::GetWeights(const N& src, const N& dest) {
  if (!IsNode(src) || !IsNode(dest)) {
    throw std::out_of_range("Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
  }

  auto src_it = std::find_if(nodes_.begin(), nodes_.end(), [&src](std::shared_ptr<Node> const& n) {return n->GetValue() == src;});
  auto dst_it = std::find_if(nodes_.begin(), nodes_.end(), [&dest](std::shared_ptr<Node> const& n) {return n->GetValue() == dest;});

  auto v1 = src_it->get()->GetWeights(dest);
  auto v2 = dst_it->get()->GetWeights(src);

  std::vector<E> v;
  v.reserve(v1.size() + v2.size());
  v.insert(v.end(), v1.begin(), v1.end());
  v.insert(v.end(), v2.begin(), v2.end());
  return v;
}

/* Node methods */
template<typename N, typename E>
gdwg::Graph<N, E>::Node::Node(N value) : value_{value} {

}

template<typename N, typename E>
N gdwg::Graph<N, E>::Node::GetValue() const {
  return this->value_;
}

template<typename N, typename E>
bool gdwg::Graph<N, E>::Node::AddEdgeTo(const std::shared_ptr<Node>& n, const E& cost) {
  std::weak_ptr<Node> edge_to = n;
  return (this->edges_to_.emplace(edge_to, cost)).second;
}

template <typename N, typename E>
bool gdwg::Graph<N,E>::Node::IsEdge(const N& dest) {
  auto dest_it = std::find_if(edges_to_.begin(), edges_to_.end(), [&dest](std::pair<std::weak_ptr<Node>, E> const& p) {
    std::shared_ptr<Node> n = p.first.lock();
    return n->GetValue() == dest;
  });
  if (dest_it != edges_to_.end()) {
    return true;
  }
  return false;
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N,E>::Node::GetEdges() {
  std::vector<N> v;

  for (auto it = edges_to_.begin(); it != edges_to_.end(); ++it) {
    auto f = it->first.lock();
    v.push_back(f->GetValue());
  }
  std::sort(v.begin(), v.end());
  return v;
}

template <typename N, typename E>
std::vector<E> gdwg::Graph<N,E>::Node::GetWeights(const N& dest) {
  std::vector<E> v;
  auto itr = std::find_if(edges_to_.begin(), edges_to_.end(), [&dest](std::pair<std::weak_ptr<Node>, E> const& p) {
    std::shared_ptr<Node> n = p.first.lock();
    return n->GetValue() == dest;
  });
  if (itr != edges_to_.end()) {
    v.push_back(itr->second);
  }
  return v;
}


