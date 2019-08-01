#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <algorithm>
#include <iterator>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace gdwg {

template<typename T>
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
  bool operator()(const P& lhs, const P& rhs) const {
    return lhs < rhs;
  }
};

template<typename N, typename E>
class Graph {
 public:
  class Node {
   public:
    Node(N);

    N GetValue() const;
    void SetValue(const N&);
    bool AddEdgeTo(const std::shared_ptr<Node>&, const E&);
    bool IsEdge(const N&) const;
    bool DeleteEdge(const N&, const E&);
    std::vector<N> GetEdges() const;
    std::vector<E> GetWeights(const N&) const;
    std::vector<std::pair<std::weak_ptr<Node>, std::set<E>>> EdgesWeights() const;

   private:
    friend class Graph;

    N value_;
    std::map<std::weak_ptr<Node>, std::set<E>, CompareByValue<Node>> edges_out_;
  };

  class const_iterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::tuple<N, N, E>;
    using reference = std::tuple<const N&, const N&, const E&>;
    using difference_type = int;

    value_type operator*() const;
    const_iterator& operator++();
    const const_iterator operator++(int);
    const_iterator& operator--();

    friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) {
      return lhs.outer_itr_ == rhs.outer_itr_ && (lhs.outer_itr_ == lhs.outer_end_itr_ || lhs.inner_itr_ == rhs.inner_itr_);
    }
    friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) {
      return !(lhs == rhs);
    }

   private:
    friend class Graph;

    typename std::set<std::shared_ptr<Node>, CompareByValue<Node>>::iterator outer_itr_;
    const typename std::set<std::shared_ptr<Node>, CompareByValue<Node>>::iterator outer_end_itr_;
    typename std::map<std::weak_ptr<Node>, std::set<E>, CompareByValue<Node>>::iterator inner_itr_;
    typename std::map<std::weak_ptr<Node>, std::set<E>, CompareByValue<Node>>::iterator inner_end_itr_;

    const_iterator(const decltype(outer_itr_)& outer, const decltype(outer_end_itr_)& outer_end, const decltype(inner_itr_)& inner, const decltype(inner_end_itr_)& inner_end): outer_itr_{outer}, outer_end_itr_{outer_end}, inner_itr_{inner}, inner_end_itr_{inner_end} {}
  };

  Graph<N,E>();
  Graph<N,E>(typename std::vector<N>::const_iterator, typename std::vector<N>::const_iterator);
  Graph<N,E>(typename std::vector<std::tuple<N,N,E>>::const_iterator, typename std::vector<std::tuple<N,N,E>>::const_iterator);
  Graph<N,E>(std::initializer_list<N>);
  Graph<N,E>(const gdwg::Graph<N,E>&);
  Graph<N,E>(gdwg::Graph<N,E>&&) noexcept;
  ~Graph<N,E>() noexcept = default;

  Graph<N,E>& operator=(const gdwg::Graph<N,E>&);
  Graph<N,E>& operator=(gdwg::Graph<N,E>&&) noexcept;

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

  template <typename F, typename U>
  friend bool operator==(const gdwg::Graph<F,U>& g1, const gdwg::Graph<F,U>& g2) {
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
  friend bool operator!=(const gdwg::Graph<F,U>& g1, const gdwg::Graph<F,U>& g2) {
    return !(g1 == g2);
  }
 
  template <typename F, typename U>
  friend std::ostream& operator<<(std::ostream& os, const Graph<F,U>& g) {
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

 private:
  std::set<std::shared_ptr<Node>, CompareByValue<Node>> nodes_;

};

}  // namespace gdwg

#include "graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
