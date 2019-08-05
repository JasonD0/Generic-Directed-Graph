#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

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

namespace gdwg {

template <typename T>
struct CompareByValue {
  bool operator()(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const {
    return lhs->GetValue() < rhs->GetValue();
  }

  bool operator()(const std::weak_ptr<T>& lhs, const std::weak_ptr<T>& rhs) const {
    std::shared_ptr<T> lhs_sp = lhs.lock();
    std::shared_ptr<T> rhs_sp = rhs.lock();
    return (lhs_sp && rhs_sp) && lhs_sp->GetValue() < rhs_sp->GetValue();
  }
};

template <typename P>
struct Comparator {
  bool operator()(const P& lhs, const P& rhs) const { return lhs < rhs; }
};

template <typename N, typename E>
class Graph {
 public:
  class Node {
   public:
    explicit Node(N);

    N GetValue() const;
    void SetValue(const N&);
    bool AddEdgeTo(const std::shared_ptr<Node>&, const E&);
    bool IsEdge(const N&) const;
    bool DeleteEdge(const N&, const E&);
    std::vector<N> GetEdges() const;
    std::vector<E> GetWeights(const N&) const;
    std::vector<std::pair<std::weak_ptr<Node>, std::set<E, Comparator<E>>>> EdgesWeights() const;

   private:
    friend class Graph;

    N value_;
    std::map<std::weak_ptr<Node>, std::set<E, Comparator<E>>, CompareByValue<Node>> edges_out_;
  };

  class const_iterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::tuple<N, N, E>;
    using reference = std::tuple<const N&, const N&, const E&>;
    using pointer = std::tuple<const N, const N, const E>*;
    using difference_type = int;

    reference operator*() const;
    const_iterator& operator++();
    const const_iterator operator++(int);
    const_iterator& operator--();
    const const_iterator operator--(int);

    friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) {
      // return lhs.outer_itr_ == rhs.outer_itr_ && (lhs.outer_itr_ == lhs.outer_end_itr_ ||
      // lhs.inner_itr_ == rhs.inner_itr_); return lhs.node_from_itr_ == rhs.node_from_itr_ &&
      // (lhs.node_from_itr_ == lhs.node_from_end_ || lhs.node_to_itr_ == rhs.node_to_itr_);
      return lhs.node_from_itr_ == rhs.node_from_itr_ &&
          (lhs.node_from_itr_ == lhs.node_from_end_ ||
              (lhs.node_to_itr_ == rhs.node_to_itr_ &&
                  (lhs.node_to_itr_ == lhs.node_to_end_ || lhs.weight_itr_ == rhs.weight_itr_)));
    }

    friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) {
      return !(lhs == rhs);
    }

   private:
    friend class Graph;

    void Next();
    void Prev(bool flag);

    typename std::set<std::shared_ptr<Node>, CompareByValue<Node>>::iterator node_from_itr_;
    const typename std::set<std::shared_ptr<Node>, CompareByValue<Node>>::iterator node_from_start_;
    const typename std::set<std::shared_ptr<Node>, CompareByValue<Node>>::iterator node_from_end_;
    typename std::map<std::weak_ptr<Node>, std::set<E, Comparator<E>>, CompareByValue<Node>>::
        iterator node_to_itr_;
    typename std::map<std::weak_ptr<Node>, std::set<E, Comparator<E>>, CompareByValue<Node>>::
        iterator node_to_start_;
    typename std::map<std::weak_ptr<Node>, std::set<E, Comparator<E>>, CompareByValue<Node>>::
        iterator node_to_end_;
    typename std::set<E, Comparator<E>>::iterator weight_itr_;
    typename std::set<E, Comparator<E>>::iterator weight_start_;
    typename std::set<E, Comparator<E>>::iterator weight_end_;

    const_iterator(const decltype(node_from_itr_)& from,
                   const decltype(node_from_start_)& from_start,
                   const decltype(node_from_end_)& from_end,
                   const decltype(node_to_itr_)& to,
                   const decltype(node_to_start_)& to_start,
                   const decltype(node_to_end_)& to_end,
                   const decltype(weight_itr_)& weight,
                   const decltype(weight_start_)& weight_start,
                   const decltype(weight_end_)& weight_end)
      : node_from_itr_{from}, node_from_start_{from_start}, node_from_end_{from_end},
        node_to_itr_{to}, node_to_start_{to_start}, node_to_end_{to_end}, weight_itr_{weight},
        weight_start_{weight_start}, weight_end_{weight_end} {}
  };

  Graph<N, E>();
  Graph<N, E>(typename std::vector<N>::const_iterator, typename std::vector<N>::const_iterator);
  Graph<N, E>(typename std::vector<std::tuple<N, N, E>>::const_iterator,
              typename std::vector<std::tuple<N, N, E>>::const_iterator);
  Graph<N, E>(std::initializer_list<N>);
  Graph<N, E>(const gdwg::Graph<N, E>&);
  Graph<N, E>(gdwg::Graph<N, E>&&) noexcept;
  ~Graph<N, E>() noexcept = default;

  Graph<N, E>& operator=(const gdwg::Graph<N, E>&);
  Graph<N, E>& operator=(gdwg::Graph<N, E>&&) noexcept;

  bool InsertNode(const N&);
  bool InsertEdge(const N&, const N&, const E&);
  bool DeleteNode(const N&);
  bool Replace(const N&, const N&);
  void MergeReplace(const N&, const N&);
  void Clear();
  bool IsNode(const N&) const;
  bool IsConnected(const N&, const N&) const;
  std::vector<N> GetNodes() const;
  std::vector<N> GetConnected(const N&) const;
  std::vector<E> GetWeights(const N&, const N&) const;
  bool erase(const N&, const N&, const E&);

  const_iterator erase(const_iterator);
  const_iterator find(const N&, const N&, const E&) const;

  const_iterator cbegin() const;
  const_iterator cend() const;
  const_iterator begin() const;
  const_iterator end() const;

  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;
  const_reverse_iterator rbegin() const;
  const_reverse_iterator rend() const;

  friend bool operator==(const gdwg::Graph<N, E>& g1, const gdwg::Graph<N, E>& g2) {
    auto it1 = g1.begin(), it2 = g2.begin();
    for (; it1 != g1.end() && it2 != g2.end(); ++it1, ++it2) {
      auto [n1_1, n1_2, c1] = *it1;
      auto [n2_1, n2_2, c2] = *it2;
      if (n1_1 != n2_1 && n1_2 != n2_2 && c1 != c2) {
        return false;
      }
    }
    return it1 == g1.end() && it2 == g2.end();
    //g2.begin();
    /*for (; it1 != g1.end(); ++it1) {
      auto [n1_1, n1_2, c1] = *it1;
      std::cout << n1_1 << " " << n1_2 << " " << c1 << "\n";
    }*/
    return true;
  }

  friend bool operator!=(const gdwg::Graph<N, E>& g1, const gdwg::Graph<N, E>& g2) {
    return !(g1 == g2);
  }

  friend std::ostream& operator<<(std::ostream& os, const Graph<N, E>& g) {
    auto nodes = g.GetNodes();
    std::reverse(nodes.begin(), nodes.end());

    N currNode = nodes.back();
    os << currNode << " (\n";
    bool matching_brackets = false;

    for (auto it = g.begin(); it != g.end(); ++it) {
      auto [src, dest, cost] = *it;
      if (currNode != src) {
        os << ")\n";
        matching_brackets = true;
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
        matching_brackets = false;
      }
      os << "  " << dest << " | " << cost << "\n";

    }
    if (!matching_brackets) {
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

 private:
  std::set<std::shared_ptr<Node>, CompareByValue<Node>> nodes_;
};

}  // namespace gdwg

#include "graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
