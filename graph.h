#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

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
};



template<typename N, typename E>
class Graph {
 public:
  class const_iterator {};

  class Node {
   public:
    Node(N);

    N GetValue() const;
    void SetValue(const N&);
    bool AddEdgeTo(const std::shared_ptr<Node>&, const E&);
    bool IsEdge(const N&);
    bool DeleteEdge(const N&, const E&);
    std::vector<N> GetEdges();
    std::vector<E> GetWeights(const N&);
    std::vector<std::pair<std::weak_ptr<Node>,E>> EdgesWeights();
    E GetWeight(const N&);

   private:
    N value_;
    // make value  vector of E
    std::map<std::weak_ptr<Node>, E, std::owner_less<std::weak_ptr<Node>>> edges_out_;
    //std::map<std::weak_ptr<Node>, E, std::owner_less<std::weak_ptr<Node>>> edges_in_;
  };

  Graph<N,E>();
  Graph<N,E>(typename std::vector<N>::const_iterator, typename std::vector<N>::const_iterator);
  Graph<N,E>(typename std::vector<std::tuple<N,N,E>>::const_iterator, typename std::vector<std::tuple<N,N,E>>::const_iterator);
  Graph<N,E>(std::initializer_list<N>);
  Graph<N,E>(const gdwg::Graph<N,E>&);
  Graph<N,E>(gdwg::Graph<N,E>&&) noexcept;
  ~Graph<N,E>() noexcept = default;

  Graph<N,E>& operator=(const gdwg::Graph<N,E>&);
  Graph<N,E>& operator=(gdwg::Graph<N,E>&&);

  bool InsertNode(const N&);
  bool InsertEdge(const N&, const N&, const E&);
  bool DeleteNode(const N&);
  bool Replace(const N&, const N&);
  void MergeReplace(const N&, const N&);
  void Clear();
  bool IsNode(const N&);
  bool IsConnected(const N&, const N&);
  std::vector<N> GetNodes();
  std::vector<N> GetConnected(const N&);
  std::vector<E> GetWeights(const N&, const N&);
  bool erase(const N&, const N&, const E&);


 private:
  std::set<std::shared_ptr<Node>, CompareByValue<Node>> nodes_;

};

}  // namespace gdwg

#include "assignments/dg/graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
