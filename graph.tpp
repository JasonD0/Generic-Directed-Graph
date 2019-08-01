#include <iterator>
#include <iostream>
#include <vector>
#include <algorithm>
#include "graph.h"


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
    std::shared_ptr<Node> n = node;
    this->nodes_.emplace(n);
  }
}

template <typename N, typename E>
gdwg::Graph<N,E>::Graph(gdwg::Graph<N,E>&& g) noexcept : nodes_{std::move(g.nodes_)} {}

template <typename N, typename E>
gdwg::Graph<N,E>& gdwg::Graph<N,E>::operator=(const gdwg::Graph<N,E>& g) {
  this->nodes_ = std::set<std::shared_ptr<Node>, CompareByValue<Node>>();
  for (const auto& node : g.nodes_) {
    std::shared_ptr<Node> n = node;
    this->nodes_.emplace(n);
  }
  return *this;
}

template <typename N, typename E>
gdwg::Graph<N,E>& gdwg::Graph<N,E>::operator=(gdwg::Graph<N,E>&& g) noexcept {
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

  auto n_it = std::find_if(nodes_.begin(), nodes_.end(), [&n](std::shared_ptr<Node> const& s) {return s->GetValue() == n;});
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

  // merge outgoing edges
  auto pair = old_it->get()->EdgesWeights();
  for (const auto& ew : pair) {
    std::shared_ptr<Node> node = ew.first.lock();
    auto n = node->GetValue();
    auto costs = ew.second;

    auto it = std::find_if(nodes_.begin(), nodes_.end(), [&n](std::shared_ptr<Node> const& s) {return s->GetValue() == n;});
    for (const auto& cost : costs) {
      new_it->get()->AddEdgeTo(*it, cost);
    }
  }

  // merge incoming edges
  for (const auto& node : nodes_) {
    if (!node->IsEdge(oldData)) {
      continue;
    }

    auto weights = node->GetWeights(oldData);

    for (const auto& weight : weights) {
      node->AddEdgeTo(*new_it, weight);
    }
  }

  nodes_.erase(old_it);
}

template <typename N, typename E>
void gdwg::Graph<N,E>::Clear() {
  for (auto itr = nodes_.begin(); itr != nodes_.end();) {
    itr = nodes_.erase(itr);
  }
}

template <typename N, typename E>
bool gdwg::Graph<N,E>::IsNode(const N& val) const {
  for (const auto& n : nodes_) {
    if (n->GetValue() == val) {
      return true;
    }
  }
  return false;
}

template <typename N, typename E>
bool gdwg::Graph<N,E>::IsConnected(const N& src, const N& dest) const {
  if (!IsNode(src) || !IsNode(dest)) {
    throw std::runtime_error("Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
  }
  auto src_it = std::find_if(nodes_.begin(), nodes_.end(), [&src](std::shared_ptr<Node> const& n) {return n->GetValue() == src;});

  return src_it->get()->IsEdge(dest);
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N,E>::GetNodes() const{
  std::vector<N> v;
  for (const auto& n : this->nodes_) {
    v.push_back(n->GetValue());
  }
  std::sort(v.begin(), v.end());
  return v;
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N,E>::GetConnected(const N& src) const {
  if (!IsNode(src)) {
    throw std::out_of_range("Cannot call Graph::GetConnected if src doesn't exist in the graph");
  }

  auto src_it = std::find_if(nodes_.begin(), nodes_.end(), [&src](std::shared_ptr<Node> const& n) {return n->GetValue() == src;});

  return src_it->get()->GetEdges();
}

template <typename N, typename E>
std::vector<E> gdwg::Graph<N,E>::GetWeights(const N& src, const N& dest) const {
  if (!IsNode(src) || !IsNode(dest)) {
    throw std::out_of_range("Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
  }

  auto src_it = std::find_if(nodes_.begin(), nodes_.end(), [&src](std::shared_ptr<Node> const& n) {return n->GetValue() == src;});

  return src_it->get()->GetWeights(dest);
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
typename gdwg::Graph<N,E>::const_iterator gdwg::Graph<N,E>::find(const N& src, const N& dest, const E& cost) const {
  for (auto it = begin(); it != end(); ++it) {
    auto [from, to, weight] = *it;
    if (src == from && dest == to && cost == weight) {
      return it;
    }
  }
  return cend();
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_iterator gdwg::Graph<N,E>::erase(const_iterator it) {
  for (auto itr = begin(); itr != end(); ++itr) {
    if (it == itr) {
      auto [src, dest, cost] = *it;
      auto tmp = ++it;
      erase(src, dest, cost);
      return tmp;
    }
  }
  return cend();
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_iterator gdwg::Graph<N,E>::cbegin() const {
  auto first = nodes_.begin();
  auto edges = first->get()->edges_out_;
  while (first != nodes_.end() && edges.begin() == edges.end()) {
    ++first;
  }
  if (first != nodes_.end()) {
    return {first, nodes_.end(), first->get()->edges_out_.begin(), first->get()->edges_out_.end()};
  }

  return cend();
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_iterator gdwg::Graph<N,E>::cend() const {
  return {nodes_.end(), nodes_.end(), {}, {}};
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_reverse_iterator gdwg::Graph<N,E>::crbegin() const {
  return rbegin();
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_reverse_iterator gdwg::Graph<N,E>::crend() const {
  return rend();
}


template <typename N, typename E>
typename gdwg::Graph<N,E>::const_iterator gdwg::Graph<N,E>::begin() const {
  return cbegin();
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_iterator gdwg::Graph<N, E>::end() const {
  return cend();
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_reverse_iterator gdwg::Graph<N,E>::rbegin() const {
  return crbegin();
}

template <typename N, typename E>
typename gdwg::Graph<N,E>::const_reverse_iterator gdwg::Graph<N,E>::rend() const {
  return crend();
}


/* iterator methods */
template<typename N, typename E>
typename gdwg::Graph<N,E>::const_iterator::value_type gdwg::Graph<N,E>::const_iterator::operator*() const {
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


template <typename F, typename U>
bool gdwg::operator==(const gdwg::Graph<F,U>& g1, const gdwg::Graph<F,U>& g2) {
  auto it1 = g1.begin(), it2 = g2.begin();
  for (; it1 != g1.end() && it2 != g2.end(); ++it1, ++it2) {
    auto [n1_1, n1_2, c1] = *it1;
    auto [n2_1, n2_2, c2] = *it2;
    if (n1_1 != n2_1 && n1_2 != n2_2 && c1 != c2) {
      return false;
    }
  }
  return !((it1 == g1.end() && it2 != g2.end()) || (it1 != g1.end() && it2 == g2.end()));
}

template <typename F, typename U>
bool gdwg::operator!=(const gdwg::Graph<F,U>& g1, const gdwg::Graph<F,U>& g2) {
  return !(g1 == g2);
}

template <typename F, typename U>
std::ostream& gdwg::operator<<(std::ostream& os, const gdwg::Graph<F,U>& g) {
  auto nodes = g.GetNodes();
  std::reverse(nodes.begin(), nodes.end());

  F currNode = nodes.back();
  os << currNode << " (\n";
  if (g.begin() == g.end()) {
    os << ")\n";
    nodes.pop_back();
  }

  int flag = 0;
  for (auto it = g.begin(); it != g.end(); ++it) {
    auto [src, dest, cost] = *it;

    if (currNode != src) {
      ++flag;
      os << ")\n";
      nodes.pop_back();

      while (currNode != src && nodes.size() != 0) {
        currNode = nodes.back();
        if (currNode != src) {
          nodes.pop_back();
          os << currNode << " (\n";
          os << ")\n";
        }
      }

      os << currNode << " (\n";
    }

    os << "  " << dest << " | " << cost <<"\n";
  }

  if (flag == 0) {
    os << ")\n";
    nodes.pop_back();
  }

  while (nodes.size() != 0) {
    currNode = nodes.back();
    nodes.pop_back();
    os << currNode << " (\n";
    os << ")\n";
  }

  return os;
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

  auto it = std::find_if(edges_out_.begin(), edges_out_.end(), [&n](std::pair<std::weak_ptr<Node>, std::set<E>> const& p) {
    std::shared_ptr<Node> node = p.first.lock();
    return node && node->GetValue() == n->GetValue();
  });

  if (it != edges_out_.end()) {
    return it->second.emplace(cost).second;
  }

  return this->edges_out_.emplace(edge_to, std::set<E>{cost}).second;
}

template <typename N, typename E>
bool gdwg::Graph<N,E>::Node::IsEdge(const N& dest) const {
  auto dest_it = std::find_if(edges_out_.begin(), edges_out_.end(), [&dest](std::pair<std::weak_ptr<Node>, std::set<E>> const& p) {
    std::shared_ptr<Node> n = p.first.lock();
    return n && n->GetValue() == dest;
  });
  return dest_it != edges_out_.end();
}

template <typename N, typename E>
bool gdwg::Graph<N,E>::Node::DeleteEdge(const N& dest, const E& cost) {
  auto dest_it = std::find_if(edges_out_.begin(), edges_out_.end(), [&dest, &cost](std::pair<std::weak_ptr<Node>, std::set<E>> const& p) {
    std::shared_ptr<Node> n = p.first.lock();
    std::set<E> c = p.second;
    return n && n->GetValue() == dest && c.find(cost) != c.end();
  });

  if (dest_it != edges_out_.end()) {
    auto &costs = dest_it->second;
    auto itr = std::find_if(costs.begin(), costs.end(), [&cost](E const& c) {return cost == c;});

    if (itr != costs.end()) {
      costs.erase(itr);
    }
    return true;
  }
  return false;
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N,E>::Node::GetEdges() const {
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
std::vector<E> gdwg::Graph<N,E>::Node::GetWeights(const N& dest) const {
  std::vector<E> v;

  auto itr = std::find_if(edges_out_.begin(), edges_out_.end(), [&dest](std::pair<std::weak_ptr<Node>, std::set<E>> const& p) {
    std::shared_ptr<Node> n = p.first.lock();
    return n && n->GetValue() == dest;
  });

  if (itr != edges_out_.end()) {
    for (const auto& cost : itr->second) {
      v.push_back(cost);
    }
  }
  return v;
}

template<typename N, typename E>
std::vector<std::pair<std::weak_ptr<typename gdwg::Graph<N,E>::Node>, std::set<E>>> gdwg::Graph<N, E>::Node::EdgesWeights() const {
   std::vector<std::pair<std::weak_ptr<Node>, std::set<E>>> v;

  for (auto it = edges_out_.begin(); it != edges_out_.end(); ++it) {
    std::pair<std::weak_ptr<Node>, std::set<E>> p{it->first, it->second};
    v.push_back(p);
  }

  return v;
}

