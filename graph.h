#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <iterator>
#include <iostream>
#include <list>
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
};

template<typename N, typename E>
class Graph {
 public:
  class const_iterator {};

  class Node {
   public:
    Node(N);

    N GetValue() const;
    void AddEdgeTo(std::shared_ptr<Node>&, E cost);

   private:
    N value_;
    //std::unordered_map<std::weak_ptr<Node>, E> edges_to_;
  };

  Graph<N,E>();
  Graph<N,E>(typename std::vector<N>::const_iterator, typename std::vector<N>::const_iterator);
  Graph<N,E>(typename std::vector<std::tuple<N,N,E>>::const_iterator, typename std::vector<std::tuple<N,N,E>>::const_iterator);
  Graph<N,E>(std::initializer_list<N>);
  Graph<N,E>(const gdwg::Graph<N,E>&);
  Graph<N,E>(gdwg::Graph<N,E>&&) noexcept;
  ~Graph<N,E>() noexcept = default;

 private:
  std::set<std::shared_ptr<Node>, CompareByValue<Node>> nodes_;

};

}  // namespace gdwg

#include "assignments/dg/graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
