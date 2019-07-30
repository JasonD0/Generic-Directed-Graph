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
    std::vector<std::pair<std::weak_ptr<Node>,E>> EdgesWeights() const;
    E GetWeight(const N&) const;

   private:
    friend class Graph;

    N value_;
    std::map<std::weak_ptr<Node>, E, CompareByValue<Node>> edges_out_;
  };

  class const_iterator {
   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::tuple<const N, const N, const E>;
    using reference = std::tuple<const N&, const N&, const E&>;
    //using pointer = std::tuple<const N, const N, const E>*;
    using difference_type = int;

    reference operator*() const;
    const_iterator& operator++();
    const const_iterator operator++(int);
    const_iterator operator--();

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
    typename std::map<std::weak_ptr<Node>, E, std::owner_less<std::weak_ptr<Node>>>::iterator inner_itr_;
    typename std::map<std::weak_ptr<Node>, E, std::owner_less<std::weak_ptr<Node>>>::iterator inner_end_itr_;

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

  template <typename M, typename R>
  friend bool operator==(const gdwg::Graph<M,R>&, const gdwg::Graph<M,R>&);
  template <typename M, typename R>
  friend bool operator!=(const gdwg::Graph<M,R>&, const gdwg::Graph<M,R>&);
  template <typename M, typename R>
  friend std::ostream& operator<<(std::ostream& os, const Graph<M,R>& g);

 private:
  std::set<std::shared_ptr<Node>, CompareByValue<Node>> nodes_;

};

template <typename M, typename R>
std::ostream& operator<<(std::ostream& os, const Graph<M,R>& g);
template <typename M, typename R>
bool operator==(const gdwg::Graph<M,R>&, const gdwg::Graph<M,R>&);
template <typename M, typename R>
bool operator!=(const gdwg::Graph<M,R>&, const gdwg::Graph<M,R>&);

}  // namespace gdwg

#include "assignments/dg/graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
