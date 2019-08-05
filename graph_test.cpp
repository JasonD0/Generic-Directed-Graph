/*

   == Explanation and rational of testing ==

   Explain and justify how you approached testing, the degree
    to which you're certain you have covered all possibilities,
    and why you think your tests are that thorough.

    Write basic test cases for each graph function. Some edge and base cases are
    implicitly included. Complex cases are ignored for simplicity. More focus on
    more complex methods e.g. iterator methods than e.g. GetNodes() because the
    more complex methods are more prone to errors. These tests
    cover around 50% of all possibilities. The complex cases with different
    orderings of method calls creating different scenarios are not covered
    extensively. Also, ost base cases aren't covered.

 */
#include "assignments/dg/graph.h"

#include <utility>

#include "catch.h"

SCENARIO("Constructors") {
  GIVEN("no arguments") {
    WHEN("a graph is constructed") {
      gdwg::Graph<std::string, int> g;

      THEN("the graph is empty") { REQUIRE(g.GetNodes().size() == 0); }
    }
  }

  GIVEN("a vector of node values") {
    std::vector<int> v{1, 2, 3};

    WHEN("a graph is constructed using the vector begin and end iterator") {
      gdwg::Graph<int, int> g{v.begin(), v.end()};

      THEN("the graph contains all the values in the vector") {
        REQUIRE(g.IsNode(1));
        REQUIRE(g.IsNode(2));
        REQUIRE(g.IsNode(3));
      }
    }
  }

  GIVEN("a vector of tuples of edges (src, dest, weight)") {
    auto e1 = std::make_tuple("s1", "s2", 5.4);
    auto e2 = std::make_tuple("s2", "s3", 7.6);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2};

    WHEN("the graph is constructed using the vector begin and end iterator") {
      gdwg::Graph<std::string, double> g{e.begin(), e.end()};

      THEN("all nodes are added to the graph and excludes duplicate nodes") {
        REQUIRE(g.IsNode("s1"));
        REQUIRE(g.IsNode("s2"));
        REQUIRE(g.IsNode("s3"));
        REQUIRE(g.GetNodes() == std::vector<std::string>{"s1", "s2", "s3"});
      }

      AND_THEN("edges with the given weights are formed") {
        REQUIRE(g.IsConnected("s1", "s2"));
        REQUIRE(g.GetWeights("s1", "s2") == std::vector<double>{5.4});
        REQUIRE(g.IsConnected("s2", "s3"));
        REQUIRE(g.GetWeights("s2", "s3") == std::vector<double>{7.6});
      }
    }
  }

  GIVEN("an initialiser list") {
    WHEN("a graph is constructed using the list") {
      gdwg::Graph<std::string, int> g{"nice", "ecin"};

      THEN("the graph contains all the values in the list") {
        REQUIRE(g.IsNode("nice"));
        REQUIRE(g.IsNode("ecin"));
      }
    }
  }

  GIVEN("a graph") {
    gdwg::Graph<std::string, int> g{"A", "B", "C"};
    g.InsertEdge("A", "C", 5);
    g.InsertEdge("B", "A", 9);

    WHEN("a graph is copy constructed from this graph") {
      auto g2{g};

      THEN("all nodes are the same") { REQUIRE(g.GetNodes() == g2.GetNodes()); }

      THEN("all edges are the same") {
        REQUIRE(g2.IsConnected("A", "C"));
        REQUIRE(g2.GetWeights("A", "C") == std::vector<int>{5});
        REQUIRE(g2.IsConnected("B", "A"));
        REQUIRE(g2.GetWeights("B", "A") == std::vector<int>{9});
      }

      WHEN("changing a value of one graph") {
        g2.Replace("A", "D");

        THEN("the other graph is also changed") {
          REQUIRE(!g2.IsNode("A"));
          REQUIRE(!g.IsNode("A"));
          REQUIRE(g2.IsNode("D"));
          REQUIRE(g.IsNode("D"));
        }
      }
    }

    WHEN("a graph is move constructed from this graph") {
      auto g2{std::move(g)};

      THEN("the new graph contains all the nodes from the old graph") {
        REQUIRE(g2.GetNodes() == std::vector<std::string>{"A", "B", "C"});
      }

      THEN("the new graph contains all edges from the old graph") {
        REQUIRE(g2.IsConnected("A", "C"));
        REQUIRE(g2.GetWeights("A", "C") == std::vector<int>{5});
        REQUIRE(g2.IsConnected("B", "A"));
        REQUIRE(g2.GetWeights("B", "A") == std::vector<int>{9});
      }
    }
  }
}

SCENARIO("Assignments") {
  GIVEN("a graph") {
    gdwg::Graph<std::string, int> g{"A", "B", "C"};
    g.InsertEdge("A", "C", 5);
    g.InsertEdge("B", "A", 9);

    WHEN("a graph is copy assigned from this graph") {
      auto g2 = g;

      THEN("all nodes are the same") { REQUIRE(g.GetNodes() == g2.GetNodes()); }

      THEN("all edges are the same") {
        REQUIRE(g2.IsConnected("A", "C"));
        REQUIRE(g2.GetWeights("A", "C") == std::vector<int>{5});
        REQUIRE(g2.IsConnected("B", "A"));
        REQUIRE(g2.GetWeights("B", "A") == std::vector<int>{9});
      }

      WHEN("changing a value of one graph") {
        g2.Replace("A", "D");

        THEN("the other graph is also changed") {
          REQUIRE(g2.IsNode("A") == false);
          REQUIRE(g.IsNode("A") == false);
          REQUIRE(g2.IsNode("D") == true);
          REQUIRE(g.IsNode("D") == true);
        }
      }
    }

    WHEN("a graph is move assigned from this graph") {
      auto g2 = std::move(g);

      THEN("the new graph contains all the nodes from the old graph") {
        REQUIRE(g2.GetNodes() == std::vector<std::string>{"A", "B", "C"});
      }

      THEN("the new graph contains all edges from the old graph") {
        REQUIRE(g2.IsConnected("A", "C"));
        REQUIRE(g2.GetWeights("A", "C") == std::vector<int>{5});
        REQUIRE(g2.IsConnected("B", "A"));
        REQUIRE(g2.GetWeights("B", "A") == std::vector<int>{9});
      }
    }
  }
}

SCENARIO("IsNode") {
  GIVEN("a graph with a node") {
    gdwg::Graph<std::string, int> g{"A"};

    WHEN("checking if the node exists in the graph") {
      bool b = g.IsNode("A");

      THEN("the result is true") { REQUIRE(b); }
    }

    WHEN("checking if a non-existing node is in the graph") {
      bool b = g.IsNode("B");

      THEN("the result is false") { REQUIRE(!b); }
    }
  }
}

SCENARIO("GetNodes") {
  GIVEN("a graph with nodes") {
    gdwg::Graph<double, int> g{1.2, 2.2, 3.4};

    WHEN("getting the nodes in the graph") {
      auto nodes = g.GetNodes();

      THEN("a vector of the graph nodes is returned") {
        REQUIRE(nodes == std::vector<double>{1.2, 2.2, 3.4});
      }
    }
  }
}

SCENARIO("InsertNode") {
  GIVEN("a graph") {
    gdwg::Graph<std::string, int> g;

    WHEN("a node is inserted") {
      bool b = g.InsertNode("A");

      THEN("the graph contains the node") {
        REQUIRE(b);
        REQUIRE(g.GetNodes() == std::vector<std::string>{"A"});
      }

      WHEN("a duplicate node is inserted") {
        bool bb = g.InsertNode("A");

        THEN("the duplicate node is not in the graph") {
          REQUIRE(!bb);
          REQUIRE(g.GetNodes() == std::vector<std::string>{"A"});
        }
      }
    }
  }
}

SCENARIO("InsertEdge") {
  GIVEN("a graph and 2 nodes") {
    gdwg::Graph<std::string, int> g{"A", "B"};

    WHEN("an edge is inserted") {
      g.InsertEdge("A", "B", 2);

      THEN("the edge is a directed edge") {
        REQUIRE(g.IsConnected("A", "B") == true);
        REQUIRE(g.GetWeights("A", "B").size() == 1);
        REQUIRE(g.GetWeights("B", "A").size() == 0);
      }
    }

    WHEN("a duplicate edge is inserted") {
      g.InsertEdge("A", "B", 2);

      THEN("the edge is not added") { REQUIRE(g.GetWeights("A", "B") == std::vector<int>{2}); }
    }

    WHEN("a different cost is added for an existing edge") {
      g.InsertEdge("A", "B", 4);

      THEN("then a new edge is added") { REQUIRE(g.GetWeights("A", "B") == std::vector<int>{4}); }
    }
  }
}

SCENARIO("Getting Edges") {
  GIVEN("a graph with edges") {
    gdwg::Graph<int, int> g{3, 5, 15};
    g.InsertEdge(3, 5, 9);
    g.InsertEdge(3, 15, -2);

    WHEN("getting the connections of a node with edges") {
      auto connections = g.GetConnected(3);

      THEN("a vector of the nodes neighbours are returned") {
        REQUIRE(connections == std::vector<int>{5, 15});
      }
    }

    WHEN("getting the connections of a node with no edges") {
      auto connections = g.GetConnected(15);

      THEN("an empty vector is returned") { REQUIRE(connections == std::vector<int>{}); }
    }
  }
}

SCENARIO("Getting Weights") {
  GIVEN("a graph with edges") {
    gdwg::Graph<int, int> g{3, 5, 15};
    g.InsertEdge(3, 5, 9);
    g.InsertEdge(3, 15, -2);

    WHEN("getting the weights of a unique connection between 2 nodes") {
      auto weight = g.GetWeights(3, 5);

      THEN("a vector containing the singular weight of the edge is returned") {
        REQUIRE(weight == std::vector<int>{9});
      }
    }

    WHEN("getting the weights of a multiple connection between 2 nodes (both directed from one "
         "node to the other)") {
      g.InsertEdge(3, 5, 999);
      auto weights = g.GetWeights(3, 5);

      THEN("a vector containing all weights of the connection is returned") {
        REQUIRE(weights == std::vector<int>{9, 999});
      }
    }
  }
}

SCENARIO("DeleteNode") {
  GIVEN("a graph and 2 edges") {
    gdwg::Graph<std::string, int> g{"A", "B", "C"};
    g.InsertEdge("A", "B", 2);
    g.InsertEdge("B", "C", 4);

    WHEN("a node is deleted") {
      g.DeleteNode("B");

      THEN("the node and its information is removed") {
        REQUIRE(!g.IsNode("B"));
        REQUIRE_THROWS_WITH(
            g.GetWeights("B", "C"),
            "Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
        REQUIRE_THROWS_WITH(
            g.IsConnected("B", "C"),
            "Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
      }
    }
  }
}

SCENARIO("Replace") {
  GIVEN("a graph and 2 edges") {
    gdwg::Graph<std::string, int> g{"A", "B", "C"};
    g.InsertEdge("A", "B", 2);
    g.InsertEdge("B", "C", 4);

    WHEN("a node is replaced with a new unique value") {
      g.Replace("B", "D");

      THEN("the node with the old value is replaced and is not in the graph") {
        REQUIRE(!g.IsNode("B"));
        REQUIRE(g.IsNode("D"));
      }

      THEN("the new value has all of old's incoming edges") {
        REQUIRE(g.IsConnected("A", "D"));
        REQUIRE(g.GetWeights("A", "D") == std::vector<int>{2});
      }

      THEN("the new value has all of old's outgoing edges") {
        REQUIRE(g.IsConnected("D", "C"));
        REQUIRE(g.GetWeights("D", "C") == std::vector<int>{4});
      }
    }
  }
}

SCENARIO("MergeReplace") {
  GIVEN("a graph and 3 edges") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("A");
    g.InsertNode("B");
    g.InsertNode("C");
    g.InsertEdge("A", "B", 2);
    g.InsertEdge("B", "C", 9);
    g.InsertEdge("B", "A", 359);
    g.InsertEdge("C", "A", 99);

    WHEN("a node is merge replaced with an existing node") {
      g.MergeReplace("A", "B");

      THEN("the replacer node contains all outgoing edges of the replaced node including its own "
           "and itself") {
        REQUIRE(!g.IsNode("A"));
        REQUIRE(g.GetConnected("B") == std::vector<std::string>{"B", "C"});
        REQUIRE(g.GetWeights("B", "B") == std::vector<int>{2, 359});
        REQUIRE(g.GetWeights("B", "C") == std::vector<int>{9});
      }

      THEN("the replacer node contains all incoming edges of the replaced node including its own "
           "and itself") {
        REQUIRE(g.GetWeights("B", "B") == std::vector<int>{2, 359});
        REQUIRE(g.GetWeights("C", "B") == std::vector<int>{99});
      }
    }

    WHEN("a duplipate edge is formed as a result of merging") {
      g.InsertEdge("B", "B", 2);
      g.MergeReplace("A", "B");

      THEN("the duplicate edge is removed") {
        REQUIRE(g.GetConnected("B") == std::vector<std::string>{"B", "C"});
        REQUIRE(g.GetWeights("B", "B") == std::vector<int>{2, 359});
      }
    }
  }
}

SCENARIO("Clear") {
  GIVEN("a graph and 2 edges") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("A");
    g.InsertNode("B");
    g.InsertNode("C");
    g.InsertEdge("A", "B", 2);
    g.InsertEdge("B", "C", 4);

    WHEN("the graph is cleared") {
      g.Clear();

      THEN("no nodes are in the graph") { REQUIRE(g.GetNodes().size() == 0); }

      WHEN("a node in the pre-cleared graph is added") {
        g.InsertNode("A");

        THEN("the node is the only node in the graph") {
          REQUIRE(g.IsNode("A"));
          REQUIRE(g.GetNodes() == std::vector<std::string>{"A"});
        }
      }

      WHEN("an edge from the pre-cleared graph is added with a different weight") {
        g.InsertNode("B");
        g.InsertNode("C");
        g.InsertEdge("B", "C", 6);

        THEN("the edge only has 1 weight") {
          REQUIRE(g.IsConnected("B", "C"));
          REQUIRE(g.GetWeights("B", "C") == std::vector<int>{6});
        }
      }
    }
  }
}

SCENARIO("find") {
  GIVEN("a graph") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("A");
    g.InsertNode("B");
    g.InsertEdge("A", "B", 2);
    g.InsertEdge("A", "B", 4);

    WHEN("finding an existing edge") {
      auto it = g.find("A", "B", 4);

      THEN("the iterator to the edge is returned") { REQUIRE(*it == std::make_tuple("A", "B", 4)); }
    }

    WHEN("finding a non-existing edge") {
      auto it1 = g.find("A", "B", 8);
      auto it2 = g.find("B", "A", 5);
      auto it3 = g.find("C", "B", 1);

      THEN("the end iterator is returned") {
        REQUIRE(it1 == g.end());
        REQUIRE(it2 == g.end());
        REQUIRE(it3 == g.end());
      }
    }
  }
}

SCENARIO("erase") {
  GIVEN("a graph") {
    gdwg::Graph<std::string, int> g;
    g.InsertNode("A");
    g.InsertNode("B");
    g.InsertEdge("A", "B", 2);
    g.InsertEdge("B", "A", 9);
    g.InsertEdge("A", "B", 4);

    WHEN("erasing an existing edge with multiple weights") {
      bool b = g.erase("A", "B", 2);

      THEN("the other weight(s) are not removed") {
        REQUIRE(b);
        REQUIRE(g.GetWeights("A", "B") == std::vector<int>{4});
      }
    }

    GIVEN("an iterator to an edge") {
      auto it = g.cbegin();
      ++it;

      WHEN("erasing an existing edge with multiple weights at the iterator") {
        auto itr = g.erase(it);

        THEN("the other weight is not removed") {
          REQUIRE(g.GetWeights("A", "B") == std::vector<int>{2});
        }

        THEN("the iterator to the next edge is returned") {
          REQUIRE(*itr == std::make_tuple("B", "A", 9));
        }

        WHEN("erasing the last edge") {
          auto end = g.erase(itr);

          THEN("the end iterator is returned") { REQUIRE(end == g.end()); }
        }
      }
    }
  }
}

SCENARIO("Graph Equality") {
  GIVEN("2 graphs with the same structure and a different graph") {
    gdwg::Graph<double, std::string> g1{5.6, 2.1, 3.3};
    gdwg::Graph<double, std::string> g2{3.3, 5.6, 2.1};
    gdwg::Graph<double, std::string> g3{9.9, 2.2, 1.1};
    g1.InsertEdge(3.3, 3.3, "six point six");
    g1.InsertEdge(5.6, 2.1, "seven point seven");
    g2.InsertEdge(5.6, 2.1, "seven point seven");
    g2.InsertEdge(3.3, 3.3, "six point six");
    g3.InsertEdge(9.9, 1.1, "eleven point zero");

    WHEN("comparing equal graphs initialised with different order") {
      bool b1 = (g1 == g2);
      bool b2 = (g1 != g2);

      THEN("the graphs are equal") {
        REQUIRE(b1 == true);
        REQUIRE(!b2);
      }
    }

    WHEN("comparing 2 different graphs") {
      bool b1 = (g1 == g3);
      bool b2 = (g1 != g3);

      THEN("the graphs are unequal") {
        REQUIRE(b1 == false);
        REQUIRE(b2 == true);
      }
    }
  }
}

SCENARIO("iterating with the first node having no edges") {
  GIVEN("an iterator to the beginning of a graph") {
    gdwg::Graph<std::string, double> g{"A", "C", "B", "D"};
    g.InsertEdge("D", "B", 2.0);
    g.InsertEdge("C", "A", 3.0);
    g.InsertEdge("D", "C", 4.0);
    g.InsertEdge("D", "C", 9.0);
    g.InsertEdge("B", "B", 5.0);
    auto it = g.begin();

    WHEN("dereferencing the iterator") {
      auto [src, dst, cst] = *it;

      THEN("the first tuple of node from, node to, and edge weight is returned") {
        REQUIRE(src == "B");
        REQUIRE(dst == "B");
        REQUIRE(cst == 5.0);
      }
    }

    WHEN("pre-incrementing the iterator") {
      ++it;

      THEN("the next tuple of node from, node to and edge weight is returned ordered by node from "
           "then node to then edge weight") {
        REQUIRE(std::get<0>(*it) == "C");
        REQUIRE(std::get<1>(*it) == "A");
        REQUIRE(std::get<2>(*it) == 3.0);
      }

      WHEN("pre-decrementing the iterator") {
        --it;

        THEN("the previous tuple of node from, node to and edge weight is returned") {
          REQUIRE(std::get<0>(*it) == "B");
          REQUIRE(std::get<1>(*it) == "B");
          REQUIRE(std::get<2>(*it) == 5.0);
        }
      }
    }

    WHEN("post-incrementing the iterator") {
      it++;

      THEN("the result is the same as pre-incrementing") {
        REQUIRE(std::get<0>(*it) == "C");
        REQUIRE(std::get<1>(*it) == "A");
        REQUIRE(std::get<2>(*it) == 3.0);
      }

      WHEN("post-decrementing the iterator") {
        it--;

        THEN("the result is the same as pre-decrementing") {
          REQUIRE(std::get<0>(*it) == "B");
          REQUIRE(std::get<1>(*it) == "B");
          REQUIRE(std::get<2>(*it) == 5.0);
        }
      }
    }

    WHEN("iterating past the last edge") {
      ++it;
      ++it;
      ++it;
      ++it;
      ++it;

      THEN("the end iterator is reached") { REQUIRE(it == g.end()); }
    }
  }
}

SCENARIO("iterating with the last node having no edges") {
  GIVEN("an iterator to the beginning of a graph") {
    gdwg::Graph<std::string, int> g{"A", "C", "B", "D"};
    g.InsertEdge("C", "A", 2);
    g.InsertEdge("A", "C", 8);
    g.InsertEdge("A", "C", 90);
    g.InsertEdge("B", "B", 50);
    auto it = g.begin();

    WHEN("iterating towards the last node") {
      ++it;
      ++it;
      auto last = ++it;
      auto end = ++it;

      THEN("the final iterator excludes the final node") {
        REQUIRE(std::get<0>(*last) == "C");
        REQUIRE(std::get<1>(*last) == "A");
        REQUIRE(std::get<2>(*last) == 2);
        REQUIRE(end == g.end());
      }
    }
  }
}

SCENARIO("iterating with a middle node having no edges") {
  GIVEN("an iterator to the beginning of a graph") {
    gdwg::Graph<int, std::string> g{4, 1, 3, 2};
    g.InsertEdge(1, 2, "three");
    g.InsertEdge(4, 2, "six");
    g.InsertEdge(4, 3, "seven");
    g.InsertEdge(2, 2, "four");
    g.InsertEdge(2, 1, "three");
    g.InsertEdge(1, 4, "five");
    auto it = g.begin();

    WHEN("iterating past a middle node with no edges") {
      ++it;
      ++it;
      auto before = ++it;
      auto after = ++it;

      THEN("the iterators before and after excludes the middle node") {
        REQUIRE(std::get<0>(*before) == 2);
        REQUIRE(std::get<1>(*before) == 2);
        REQUIRE(std::get<2>(*before) == "four");

        REQUIRE(std::get<0>(*after) == 4);
        REQUIRE(std::get<1>(*after) == 2);
        REQUIRE(std::get<2>(*after) == "six");
      }
    }
  }
}

SCENARIO("reverse iterating with the first node having no edges") {
  GIVEN("an iterator to the end of a graph") {
    gdwg::Graph<std::string, double> g{"A", "C", "B", "D"};
    g.InsertEdge("D", "B", 2.0);
    g.InsertEdge("C", "A", 3.0);
    g.InsertEdge("D", "C", 4.0);
    g.InsertEdge("D", "C", 9.0);
    g.InsertEdge("B", "B", 5.0);
    auto it = g.rbegin();

    WHEN("dereferencing the iterator") {
      auto [src, dst, cst] = *it;

      THEN("the last tuple of node from, node to, and edge weight is returned") {
        REQUIRE(src == "D");
        REQUIRE(dst == "C");
        REQUIRE(cst == 9.0);
      }
    }

    WHEN("pre-incrementing the iterator") {
      ++it;

      THEN("the previous tuple of node from, node to and edge weight is returned ordered by node from then node to then edge weight") {
        REQUIRE(std::get<0>(*it) == "D");
        REQUIRE(std::get<1>(*it) == "C");
        REQUIRE(std::get<2>(*it) == 4.0);
      }

      /*WHEN("pre-decrementing the iterator") {
        --it;

        THEN("the next tuple of node from, node to and edge weight is returned") {
          REQUIRE(std::get<0>(*it) == "D");
          REQUIRE(std::get<1>(*it) == "C");
          REQUIRE(std::get<2>(*it) == 9.0);
        }
      }*/
    }

    WHEN("post-incrementing the iterator") {
      it++;

      THEN("the result is the same as pre-incrementing") {
        REQUIRE(std::get<0>(*it) == "D");
        REQUIRE(std::get<1>(*it) == "C");
        REQUIRE(std::get<2>(*it) == 4.0);
      }

      /*WHEN("post-decrementing the iterator") {
        it--;

        THEN("the result is the same as pre-decrementing") {
          REQUIRE(std::get<0>(*it) == "D");
          REQUIRE(std::get<1>(*it) == "C");
          REQUIRE(std::get<2>(*it) == 9.0);
        }
      }*/
    }

    WHEN("iterating past the first edge") {
      ++it;
      ++it;
      ++it;
      ++it;
      ++it;

      THEN("the end iterator is reached") { REQUIRE(it == g.rend()); }
    }
  }
}
SCENARIO("reverse iterating with the last node (highest value) having no edges") {
  GIVEN("an iterator to the end of a graph") {
    gdwg::Graph<std::string, int> g{"A", "C", "B", "D"};
    g.InsertEdge("C", "A", 2);
    g.InsertEdge("A", "C", 8);
    g.InsertEdge("A", "C", 90);
    g.InsertEdge("B", "B", 50);
    auto it = g.rbegin();

    WHEN("iterating towards the last node") {
      ++it;
      ++it;
      auto last = ++it;
      auto end = ++it;

      THEN("the final iterator excludes the final node") {
        REQUIRE(std::get<0>(*last) == "A");
        REQUIRE(std::get<1>(*last) == "C");
        REQUIRE(std::get<2>(*last) == 8);
        REQUIRE(end == g.rend());
      }
    }
  }
}


SCENARIO("reverse iterating with a middle node having no edges") {
  GIVEN("an iterator to the end of a graph") {
    gdwg::Graph<int, std::string> g{4, 1, 3, 2};
    g.InsertEdge(1, 2, "three");
    g.InsertEdge(4, 2, "six");
    g.InsertEdge(4, 3, "seven");
    g.InsertEdge(2, 2, "four");
    g.InsertEdge(2, 1, "three");
    g.InsertEdge(1, 4, "five");
    auto it = g.rbegin();

    WHEN("iterating past a middle node with no edges") {
      ++it;
      ++it;
      auto before = ++it;
      auto after = ++it;

      THEN("the iterators before and after excludes the middle node") {
        REQUIRE(std::get<0>(*before) == 2);
        REQUIRE(std::get<1>(*before) == 1);
        REQUIRE(std::get<2>(*before) == "three");

        REQUIRE(std::get<0>(*after) == 1);
        REQUIRE(std::get<1>(*after) == 4);
        REQUIRE(std::get<2>(*after) == "five");
      }
    }
  }
}
