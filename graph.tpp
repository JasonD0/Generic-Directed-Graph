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
    InsertNode(src);
    InsertNode(dest);
    InsertEdge(src, dest, cost);
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
  for (const auto& node : g.nodes_) {
    this->nodes_.emplace(std::make_shared(node));
  }
}

template <typename N, typename E>
gdwg::Graph<N,E>::Graph(gdwg::Graph<N,E>&& g) noexcept : nodes_{std::move(g.nodes_)} {}

template <typename N, typename E>
gdwg::Graph<N,E>& gdwg::Graph<N,E>::operator=(const gdwg::Graph<N,E>& g) {
  this->nodes_ = std::set<std::shared_ptr<Node>, CompareByValue<Node>>();
  for (const auto& node : g.nodes_) {
    this->nodes_.emplace(std::make_shared<Node>(node));
  }
  return *this;
}

template <typename N, typename E>
gdwg::Graph<N,E>& gdwg::Graph<N,E>::operator=(gdwg::Graph<N,E>&& g) {
  this->nodes_ = std::move(g.nodes_);
  return *this;
}

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
bool gdwg::Graph<N,E>::DeleteNode(const N& n) {
  if (!IsNode(n)) {
    return false;
  }

  auto n_it = std::find_if(nodes_.begin(), nodes_.end(), [&n](std::shared_ptr<Node> const& s) {return s->GetValue();});
  nodes_.erase(n_it);

  return true;
}

template <typename N, typename E>
bool gdwg::Graph<N,E>::Replace(const N& oldData, const N& newData) {
  if (!IsNode(oldData)) {
    throw std::runtime_error("Cannot call Graph::Replace on a node that doesn't exist");
  }

  if (IsNode(newData)) {
    return false;
  }

  auto it = std::find_if(nodes_.begin(), nodes_.end(), [&oldData](std::shared_ptr<Node> const& n) {return n->GetValue() == oldData;});
  it->get()->SetValue(newData);

  return true;
}

template <typename N, typename E>
void gdwg::Graph<N,E>::MergeReplace(const N& oldData, const N& newData) {
  if (!IsNode(oldData) || !IsNode(newData)) {
    throw std::runtime_error("Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
  }

  auto old_it = std::find_if(nodes_.begin(), nodes_.end(), [&oldData](std::shared_ptr<Node> const& n) {return n->GetValue() == oldData;});
  auto new_it = std::find_if(nodes_.begin(), nodes_.end(), [&newData](std::shared_ptr<Node> const& n) {return n->GetValue() == newData;});

  nodes_.erase(old_it);

  // outgoing edges
  auto pair = old_it->get()->EdgesWeights();
  for (const auto& ew : pair) {
    std::shared_ptr<Node> node = ew.first.lock();
    auto n = node->GetValue();
    auto cost = ew.second;
    auto it = std::find_if(nodes_.begin(), nodes_.end(), [&n](std::shared_ptr<Node> const& s) {return s->GetValue() == n;});
    new_it->get()->AddEdgeTo(*it, cost);
  }

  // incoming edges
  for (const auto& node : nodes_) {
    if (!node->IsEdge(newData)) {
      continue;
    }
    node->AddEdgeTo(*new_it, node->GetWeight(newData));
  }
}

template <typename N, typename E>
void gdwg::Graph<N,E>::Clear() {
  for (auto itr = nodes_.begin(); itr != nodes_.end();) {
    itr = nodes_.erase(itr);
  }
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

  auto src_it = std::find_if(nodes_.begin(), nodes_.end(), [&src](std::shared_ptr<Node> const& n) {return n->GetValue() == src;});
  src_it->get()->GetEdges();
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

template <typename N, typename E>
bool gdwg::Graph<N,E>::erase(const N& src, const N& dest, const E& w) {
  if (!IsNode(src) || !IsNode(dest)) {
    return false;
  }

  auto src_itr = std::find_if(nodes_.begin(), nodes_.end(), [&src](std::shared_ptr<Node> const& n) {return n->GetValue() == src;});

  return src_itr->get()->DeleteEdge(dest, w);
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_iterator gdwg::Graph<N,E>::find(const N& src, const N& dest, const E& cost) {
  for (auto it = begin(); it != end(); ++it) {
    auto nodeFrom = std::get<0>(*it);
    auto nodeTo = std::get<1>(*it);
    auto edgeCost = std::get<2>(*it);
    if (src == nodeFrom && dest == nodeTo && cost == edgeCost) {
      return it;
    }
  }
  return cend();
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_iterator gdwg::Graph<N,E>::erase(const_iterator it) {
  for (auto itr = begin(); itr != end(); ++itr) {
    if (it == itr) {
      auto nodeFrom = std::get<0>(*it);
      auto nodeTo = std::get<1>(*it);
      auto edgeCost = std::get<2>(*it);
      auto tmp = ++it;
      erase(nodeFrom, nodeTo, edgeCost);
      return tmp;
    }
  }
  return cend();
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_iterator gdwg::Graph<N,E>::cbegin() {
  auto first = nodes_.begin();
  if (first != nodes_.end()) {
    return {first, nodes_.end(), first->get()->edges_out_.begin(), first->get()->edges_out_.end()};
  }
  return cend();
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_iterator gdwg::Graph<N,E>::cend() {
  return {nodes_.end(), nodes_.end(), {}, {}};
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_reverse_iterator gdwg::Graph<N,E>::crbegin() {
  return rbegin();
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_reverse_iterator gdwg::Graph<N,E>::crend() {
  return rend();
}


template <typename N, typename E>
typename gdwg::Graph<N,E>::const_iterator gdwg::Graph<N,E>::begin() {
  return cbegin();
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_iterator gdwg::Graph<N, E>::end() {
  return cend();
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_reverse_iterator gdwg::Graph<N,E>::rbegin() {
  return crbegin();
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_reverse_iterator gdwg::Graph<N,E>::rend() {
  return crend();
}


/* iterator methods */
template<typename N, typename E>
typename gdwg::Graph<N,E>::const_iterator::reference gdwg::Graph<N,E>::const_iterator::operator*() const {
  const auto& nodeFrom = outer_itr_->get()->GetValue();
  const auto& nodeTo = inner_itr_->first.lock()->GetValue();
  const auto& cost = inner_itr_->second;
  return {nodeFrom, nodeTo, cost};
}

template<typename N, typename E>
typename gdwg::Graph<N,E>::const_iterator& gdwg::Graph<N,E>::const_iterator::operator++() {
  ++inner_itr_;
  if (inner_itr_ == inner_end_itr_) {
    do {
      ++outer_itr_;
      if (outer_itr_ != outer_end_itr_) {
        inner_itr_ = outer_itr_->get()->edges_out_.begin();
        inner_end_itr_ = outer_itr_->get()->edges_out_.end();
      }
    } while (outer_itr_ != outer_end_itr_ && inner_itr_ == inner_end_itr_);
  }
  return *this;
}

template<typename N, typename E>
const typename gdwg::Graph<N,E>::const_iterator gdwg::Graph<N,E>::const_iterator::operator++(int) {
  auto tmp{*this};
  ++(*this);
  return tmp;
}





/* Node methods */
template<typename N, typename E>
gdwg::Graph<N, E>::Node::Node(N value) : value_{value} {

}

template<typename N, typename E>
N gdwg::Graph<N, E>::Node::GetValue() const {
  return this->value_;
}

template <typename N, typename E>
void gdwg::Graph<N,E>::Node::SetValue(const N& newData) {
  this->value_ = newData;
}

template<typename N, typename E>
bool gdwg::Graph<N, E>::Node::AddEdgeTo(const std::shared_ptr<Node>& n, const E& cost) {
  std::weak_ptr<Node> edge_to = n;
  return (this->edges_out_.emplace(edge_to, cost)).second;
}

template <typename N, typename E>
bool gdwg::Graph<N,E>::Node::IsEdge(const N& dest) {
  auto dest_it = std::find_if(edges_out_.begin(), edges_out_.end(), [&dest](std::pair<std::weak_ptr<Node>, E> const& p) {
    std::shared_ptr<Node> n = p.first.lock();
    return n && n->GetValue() == dest;
  });
  if (dest_it != edges_out_.end()) {
    return true;
  }
  return false;
}

template <typename N, typename E>
bool gdwg::Graph<N,E>::Node::DeleteEdge(const N& dest, const E& cost) {
  auto dest_it = std::find_if(edges_out_.begin(), edges_out_.end(), [&dest, &cost](std::pair<std::weak_ptr<Node>, E> const& p) {
    std::shared_ptr<Node> n = p.first.lock();
    E c = p.second;
    return n && n->GetValue() == dest && c == cost;
  });
  if (dest_it != edges_out_.end()) {
    edges_out_.erase(dest_it->first);
    return true;
  }
  return false;
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N,E>::Node::GetEdges() {
  std::vector<N> v;

  for (auto it = edges_out_.begin(); it != edges_out_.end(); ++it) {
    auto wp = it->first.lock();
    if (wp) {
      v.push_back(wp->GetValue());
    }
  }

  std::sort(v.begin(), v.end());
  return v;
}

template <typename N, typename E>
std::vector<E> gdwg::Graph<N,E>::Node::GetWeights(const N& dest) {
  std::vector<E> v;
  auto itr = std::find_if(edges_out_.begin(), edges_out_.end(), [&dest](std::pair<std::weak_ptr<Node>, E> const& p) {
    std::shared_ptr<Node> n = p.first.lock();
    return n && n->GetValue() == dest;
  });
  if (itr != edges_out_.end()) {
    v.push_back(itr->second);
  }
  return v;
}

template<typename N, typename E>
std::vector<std::pair<std::weak_ptr<typename gdwg::Graph<N,E>::Node>, E>> gdwg::Graph<N, E>::Node::EdgesWeights() {
   std::vector<std::pair<std::weak_ptr<Node>, E>> v;

  for (auto it = edges_out_.begin(); it != edges_out_.end(); ++it) {
    std::pair<std::weak_ptr<Node>, E> p{it->first, it->second};
    v.push_back(p);
  }

  return v;
}

template <typename N, typename E>
E gdwg::Graph<N,E>::Node::GetWeight(const N& n) {
  auto it = std::find_if(edges_out_.begin(), edges_out_.end(), [&n](std::pair<std::weak_ptr<Node>, E> const& p) {
    std::shared_ptr<Node> sp = p.first.lock();
    return sp->GetValue() == n;
  });
  return it->second;
}

