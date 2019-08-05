#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

template <typename N, typename E>
gdwg::Graph<N, E>::Graph() : nodes_{std::set<std::shared_ptr<Node>, CompareByValue<Node>>{}} {}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<N>::const_iterator begin,
                         typename std::vector<N>::const_iterator end)
  : nodes_{std::set<std::shared_ptr<Node>, CompareByValue<Node>>{}} {
  int len = end - begin;
  for (int i = 0; i < len; ++i) {
    nodes_.emplace(std::make_shared<Node>(*begin++));
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<std::tuple<N, N, E>>::const_iterator begin,
                         typename std::vector<std::tuple<N, N, E>>::const_iterator end)
  : nodes_{std::set<std::shared_ptr<Node>, CompareByValue<Node>>{}} {
  int len = end - begin;
  for (int i = 0; i < len; ++i) {
    auto [src, dest, cost] = *begin++;
    InsertNode(src);
    InsertNode(dest);
    InsertEdge(src, dest, cost);
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(std::initializer_list<N> list)
  : nodes_{std::set<std::shared_ptr<Node>, CompareByValue<Node>>{}} {
  for (const auto& n : list) {
    auto ptr = std::make_shared<Node>(n);
    nodes_.emplace(ptr);
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(const gdwg::Graph<N, E>& g)
  : nodes_{std::set<std::shared_ptr<Node>, CompareByValue<Node>>{}} {
  for (const auto& node : g.nodes_) {
    std::shared_ptr<Node> n = node;
    this->nodes_.emplace(n);
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(gdwg::Graph<N, E>&& g) noexcept : nodes_{std::move(g.nodes_)} {}

template <typename N, typename E>
gdwg::Graph<N, E>& gdwg::Graph<N, E>::operator=(const gdwg::Graph<N, E>& g) {
  this->nodes_ = std::set<std::shared_ptr<Node>, CompareByValue<Node>>();
  for (const auto& node : g.nodes_) {
    std::shared_ptr<Node> n = node;
    this->nodes_.emplace(n);
  }
  return *this;
}

template <typename N, typename E>
gdwg::Graph<N, E>& gdwg::Graph<N, E>::operator=(gdwg::Graph<N, E>&& g) noexcept {
  this->nodes_ = std::move(g.nodes_);
  return *this;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::InsertNode(const N& val) {
  auto new_node = std::make_shared<Node>(val);
  auto inserted = this->nodes_.emplace(new_node);
  return inserted.second;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::InsertEdge(const N& src, const N& dest, const E& w) {
  if (!IsNode(src) || !IsNode(dest)) {
    throw std::runtime_error(
        "Cannot call Graph::InsertEdge when either src or dst node does not exist");
  }

  auto src_it = std::find_if(nodes_.begin(), nodes_.end(), [&src](std::shared_ptr<Node> const& n) {
    return n->GetValue() == src;
  });
  auto dst_it = std::find_if(nodes_.begin(), nodes_.end(), [&dest](std::shared_ptr<Node> const& n) {
    return n->GetValue() == dest;
  });

  return src_it->get()->AddEdgeTo(*dst_it, w);
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::DeleteNode(const N& n) {
  if (!IsNode(n)) {
    return false;
  }

  auto n_it = std::find_if(nodes_.begin(), nodes_.end(),
                           [&n](std::shared_ptr<Node> const& s) { return s->GetValue() == n; });
  nodes_.erase(n_it);

  return true;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::Replace(const N& oldData, const N& newData) {
  if (!IsNode(oldData)) {
    throw std::runtime_error("Cannot call Graph::Replace on a node that doesn't exist");
  }

  if (IsNode(newData)) {
    return false;
  }

  auto it = std::find_if(nodes_.begin(), nodes_.end(), [&oldData](std::shared_ptr<Node> const& n) {
    return n->GetValue() == oldData;
  });
  it->get()->SetValue(newData);

  return true;
}

template <typename N, typename E>
void gdwg::Graph<N, E>::MergeReplace(const N& oldData, const N& newData) {
  if (!IsNode(oldData) || !IsNode(newData)) {
    throw std::runtime_error(
        "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
  }

  auto old_it =
      std::find_if(nodes_.begin(), nodes_.end(),
                   [&oldData](std::shared_ptr<Node> const& n) { return n->GetValue() == oldData; });
  auto new_it =
      std::find_if(nodes_.begin(), nodes_.end(),
                   [&newData](std::shared_ptr<Node> const& n) { return n->GetValue() == newData; });

  // merge outgoing edges
  auto pair = old_it->get()->EdgesWeights();
  for (const auto& ew : pair) {
    std::shared_ptr<Node> node = ew.first.lock();
    auto n = node->GetValue();
    auto costs = ew.second;

    auto it = std::find_if(nodes_.begin(), nodes_.end(),
                           [&n](std::shared_ptr<Node> const& s) { return s->GetValue() == n; });
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
void gdwg::Graph<N, E>::Clear() {
  for (auto itr = nodes_.begin(); itr != nodes_.end();) {
    itr = nodes_.erase(itr);
  }
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::IsNode(const N& val) const {
  for (const auto& n : nodes_) {
    if (n->GetValue() == val) {
      return true;
    }
  }
  return false;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::IsConnected(const N& src, const N& dest) const {
  if (!IsNode(src) || !IsNode(dest)) {
    throw std::runtime_error(
        "Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
  }
  auto src_it = std::find_if(nodes_.begin(), nodes_.end(), [&src](std::shared_ptr<Node> const& n) {
    return n->GetValue() == src;
  });

  return src_it->get()->IsEdge(dest);
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetNodes() const {
  std::vector<N> v;
  v.reserve(nodes_.size());
  for (const auto& n : this->nodes_) {
    v.push_back(n->GetValue());
  }
  std::sort(v.begin(), v.end());
  return v;
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetConnected(const N& src) const {
  if (!IsNode(src)) {
    throw std::out_of_range("Cannot call Graph::GetConnected if src doesn't exist in the graph");
  }

  auto src_it = std::find_if(nodes_.begin(), nodes_.end(), [&src](std::shared_ptr<Node> const& n) {
    return n->GetValue() == src;
  });

  return src_it->get()->GetEdges();
}

template <typename N, typename E>
std::vector<E> gdwg::Graph<N, E>::GetWeights(const N& src, const N& dest) const {
  if (!IsNode(src) || !IsNode(dest)) {
    throw std::out_of_range(
        "Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
  }

  auto src_it = std::find_if(nodes_.begin(), nodes_.end(), [&src](std::shared_ptr<Node> const& n) {
    return n->GetValue() == src;
  });

  return src_it->get()->GetWeights(dest);
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::erase(const N& src, const N& dest, const E& w) {
  if (!IsNode(src) || !IsNode(dest)) {
    return false;
  }

  auto src_itr = std::find_if(nodes_.begin(), nodes_.end(), [&src](std::shared_ptr<Node> const& n) {
    return n->GetValue() == src;
  });

  return src_itr->get()->DeleteEdge(dest, w);
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator
gdwg::Graph<N, E>::find(const N& src, const N& dest, const E& cost) const {
  for (auto it = begin(); it != end(); ++it) {
    auto [from, to, weight] = *it;
    if (src == from && dest == to && cost == weight) {
      return it;
    }
  }
  return cend();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::erase(const_iterator it) {
  for (auto itr = begin(); itr != end(); ++itr) {
    if (it == itr) {
      auto [src, dest, cost] = *itr;
      auto tmp = ++itr;
      erase(src, dest, cost);
      return tmp;
    }
  }
  return cend();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::cbegin() const {
  auto first = nodes_.begin();

  // get first node_from  with node_to valid
  auto edges = first->get()->edges_out_;

  // get first node source with connections
  while (first != nodes_.end() && edges.begin() == edges.end()) {
    ++first;
    edges = first->get()->edges_out_;
  }

  // get first node destination skipping invalid weak pointers (owner was deleted)
  if (first != nodes_.end()) {
    std::shared_ptr<Node> sp = edges.begin()->first.lock();
    while (first != nodes_.end() && !sp) {
      ++first;
      edges = first->get()->edges_out_;
      sp = edges.begin()->first.lock();
    }
  }

  if (first != nodes_.end()) {
    return {first,
            std::prev(first),
            //nodes_.begin(),
            nodes_.end(),

            first->get()->edges_out_.begin(),
            //std::prev(first->get()->edges_out_.begin()),
            first->get()->edges_out_.begin(),
            first->get()->edges_out_.end(),

            first->get()->edges_out_.begin()->second.begin(),
            //std::prev(first->get()->edges_out_.begin()->second.begin()),
            first->get()->edges_out_.begin()->second.begin(),
            first->get()->edges_out_.begin()->second.end()};
  }

  return cend();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::cend() const {
  return {nodes_.end(), nodes_.begin(), nodes_.end(), {}, {}, {}, {}, {}, {}};
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_reverse_iterator gdwg::Graph<N, E>::crbegin() const {
  return const_reverse_iterator(cend());
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_reverse_iterator gdwg::Graph<N, E>::crend() const {
  return const_reverse_iterator(cbegin());
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::begin() const {
  return cbegin();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::end() const {
  return cend();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_reverse_iterator gdwg::Graph<N, E>::rbegin() const {
  return crbegin();
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_reverse_iterator gdwg::Graph<N, E>::rend() const {
  return crend();
}

/* iterator methods */
template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator::reference gdwg::Graph<N, E>::const_iterator::
operator*() const {
  const auto& nodeFrom = node_from_itr_->get()->value_;
  const auto& nodeTo = node_to_itr_->first.lock()->value_;
  const auto& cost = *weight_itr_;

  return {nodeFrom, nodeTo, cost};
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator& gdwg::Graph<N, E>::const_iterator::operator++() {
  ++weight_itr_;
  if (weight_itr_ == weight_end_) {
    ++node_to_itr_;

    // no more edges for current src
    // find src node with edges
    if (node_to_itr_ == node_to_end_) {
      Next();

      // still have edges for current src
    } else {
      // check weak pointer still valid
      // get first valid dest with edges
      std::shared_ptr<Node> sp = node_to_itr_->first.lock();  // get map key (weak pointer)
      while (node_to_itr_ != node_to_end_ && !sp) {
        ++node_to_itr_;
        sp = (node_to_itr_ != node_to_end_) ? node_to_itr_->first.lock() : sp;
      }

      // found valid   weight itr
      if (sp) {
        weight_itr_ = node_to_itr_->second.begin();
        //weight_start_ = std::prev(node_to_itr_->second.begin());
        weight_start_ = node_to_itr_->second.begin();
        weight_end_ = node_to_itr_->second.end();

        // no edges for current
      } else {
        Next();
      }
    }
  }
  return *this;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::const_iterator::Next() {
  do {
    ++node_from_itr_;

    if (node_from_itr_ != node_from_end_) {

      node_to_itr_ = node_from_itr_->get()->edges_out_.begin();
      node_to_end_ = node_from_itr_->get()->edges_out_.end();
      //node_to_start_ = std::prev(node_from_itr_->get()->edges_out_.begin());
      node_to_start_ = node_from_itr_->get()->edges_out_.begin();

      if (node_to_itr_ == node_to_end_) {
        continue;
      }

      std::shared_ptr<Node> sp = node_to_itr_->first.lock();

      if (!sp) {
        node_to_itr_ = node_to_end_;  // owner of weak pointer dead  reset loop

      } else {
        // found edge   weight itr
        weight_itr_ = node_to_itr_->second.begin();
        //weight_start_ = std::prev(node_to_itr_->second.begin());
        weight_start_ = node_to_itr_->second.begin();
        weight_end_ = node_to_itr_->second.end();
      }
    }
  } while (node_from_itr_ != node_from_end_ && node_to_itr_ == node_to_end_);

  return node_from_itr_ != node_from_end_;
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator& gdwg::Graph<N, E>::const_iterator::operator--() {

  if (weight_itr_ == weight_start_) {   // trying to decrement start of weight

    // when through last edge weight   if current node_to  is at beginning -> dont decrement -> change node_from
    if (node_to_itr_ == node_to_start_) {
      Prev();

    } else {
      --node_to_itr_;
    }

    // check weak pointer still valid (owner not deleted)
    std::shared_ptr<Node> sp = node_to_itr_->first.lock();  // get map key (weak pointer)
    while (node_to_itr_ != node_to_start_ && !sp) {
      --node_to_itr_;
      sp = node_to_itr_->first.lock();
    }

    // reached 1st element
    if (node_to_itr_ == node_to_start_) {
      sp = node_to_itr_->first.lock();
    }

    // found valid   weight itr
    if (sp) {
      weight_itr_ = std::prev(node_to_itr_->second.end());      // get last weight
      //weight_start_ = std::prev(node_to_itr_->second.begin());  // set end pt
      weight_start_ = node_to_itr_->second.begin();
      weight_end_ = node_to_itr_->second.end();

      // no edges for current
    } else {
      Prev();
    }

  } else {
    --weight_itr_;
  }

  return *this;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::const_iterator::Prev() {
  do {
    --node_from_itr_;

    if (node_from_itr_ != node_from_start_) {

      node_to_itr_ = std::prev(node_from_itr_->get()->edges_out_.end());      // last element
      node_to_end_ = node_from_itr_->get()->edges_out_.end();
      //node_to_start_ = std::prev(node_from_itr_->get()->edges_out_.begin());
      node_to_start_ = node_from_itr_->get()->edges_out_.begin();

      std::shared_ptr<Node> sp = node_to_itr_->first.lock();
      if (!sp) {
        node_to_start_ = node_to_end_;  // owner of weak pointer dead  reset loop

      } else {
        // found edge   weight itr
        weight_itr_ = std::prev(node_to_itr_->second.end());      // last element
        //weight_start_ = std::prev(node_to_itr_->second.begin());  // end pt
        weight_start_ = node_to_itr_->second.begin();
        weight_end_ = node_to_itr_->second.end();
      }
    }
  } while (node_from_itr_ != node_from_start_ && node_to_start_ == node_to_end_); // while not finished with node from and node to is empty (ie no edges)

  return node_from_itr_ != node_from_start_;
}

template <typename N, typename E>
const typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::const_iterator::
operator++(int) {
  auto tmp{*this};
  ++(*this);
  return tmp;
}

template <typename N, typename E>
const typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::const_iterator::
operator--(int) {
  auto tmp{*this};
  --(*this);
  return tmp;
}

/* Node methods */
template <typename N, typename E>
gdwg::Graph<N, E>::Node::Node(N value) : value_{value} {}

template <typename N, typename E>
N gdwg::Graph<N, E>::Node::GetValue() const {
  return this->value_;
}

template <typename N, typename E>
void gdwg::Graph<N, E>::Node::SetValue(const N& newData) {
  this->value_ = newData;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::Node::AddEdgeTo(const std::shared_ptr<Node>& n, const E& cost) {
  std::weak_ptr<Node> edge_to = n;

  auto it = std::find_if(edges_out_.begin(), edges_out_.end(),
                         [&n](std::pair<std::weak_ptr<Node>, std::set<E, Comparator<E>>> const& p) {
                           std::shared_ptr<Node> node = p.first.lock();
                           return node && node->GetValue() == n->GetValue();
                         });

  if (it != edges_out_.end()) {
    return it->second.emplace(cost).second;
  }

  return this->edges_out_.emplace(edge_to, std::set<E, Comparator<E>>{cost}).second;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::Node::IsEdge(const N& dest) const {
  auto dest_it =
      std::find_if(edges_out_.begin(), edges_out_.end(),
                   [&dest](std::pair<std::weak_ptr<Node>, std::set<E, Comparator<E>>> const& p) {
                     std::shared_ptr<Node> n = p.first.lock();
                     return n && n->GetValue() == dest;
                   });
  return dest_it != edges_out_.end();
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::Node::DeleteEdge(const N& dest, const E& cost) {
  auto dest_it = std::find_if(
      edges_out_.begin(), edges_out_.end(),
      [&dest, &cost](std::pair<std::weak_ptr<Node>, std::set<E, Comparator<E>>> const& p) {
        std::shared_ptr<Node> n = p.first.lock();
        std::set<E, Comparator<E>> c = p.second;
        return n && n->GetValue() == dest && c.find(cost) != c.end();
      });

  if (dest_it != edges_out_.end()) {
    auto& costs = dest_it->second;
    auto itr = std::find_if(costs.begin(), costs.end(), [&cost](E const& c) { return cost == c; });

    if (itr != costs.end()) {
      costs.erase(itr);
    }
    return true;
  }
  return false;
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::Node::GetEdges() const {
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
std::vector<E> gdwg::Graph<N, E>::Node::GetWeights(const N& dest) const {
  std::vector<E> v;

  auto itr =
      std::find_if(edges_out_.begin(), edges_out_.end(),
                   [&dest](std::pair<std::weak_ptr<Node>, std::set<E, Comparator<E>>> const& p) {
                     std::shared_ptr<Node> n = p.first.lock();
                     return n && n->GetValue() == dest;
                   });

  if (itr != edges_out_.end()) {
    v.reserve(itr->second.size());
    for (const auto& cost : itr->second) {
      v.push_back(cost);
    }
  }
  return v;
}

template <typename N, typename E>
std::vector<
    std::pair<std::weak_ptr<typename gdwg::Graph<N, E>::Node>, std::set<E, gdwg::Comparator<E>>>>
gdwg::Graph<N, E>::Node::EdgesWeights() const {
  std::vector<std::pair<std::weak_ptr<Node>, std::set<E, Comparator<E>>>> v;

  for (auto it = edges_out_.begin(); it != edges_out_.end(); ++it) {
    std::pair<std::weak_ptr<Node>, std::set<E, Comparator<E>>> p{it->first, it->second};
    v.push_back(p);
  }

  return v;
}
