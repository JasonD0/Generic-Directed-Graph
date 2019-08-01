/*

  == Explanation and rational of testing ==

  Explain and justify how you approached testing, the degree
   to which you're certain you have covered all possibilities,
   and why you think your tests are that thorough.

*/

#include "assignments/dg/graph.h"
#include "catch.h"

SCENARIO("Constructors") {
  GIVEN("no arguments") {

    WHEN("a graph is constructed") {
      gdwg::Graph<std::string, int> g;

      THEN("the graph is empty") {
        REQUIRE(g.GetNodes().size == 0);
      }
    }
  }

  GIVEN("a vector of node values") {
    std::vector<int> v{1, 2, 3};

    WHEN("a graph is constructed using the vector begin and end iterator") {
      gdwg::Graph<std::string, int> g{v.begin(), v.end()};

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
      gdwg::Graph<std::string, double> b{e.begin(), e.end()};

      THEN("all nodes are added to the graph and exlcudes duplicate nodes") {
        REQUIRE(g.IsNode("s1"));
        REQUIRE(g.IsNode("s2"));
        REQUIRE(g.IsNode("s3"));
        REQUIRE(g.GetNodes() == std::vector<std::string>{"s1", "s2", "s3"});
      }

      AND_THEN("edges with the given weights are formed") {
        REQUIRE(g.IsConnected("s1", "s2"));
        REQUIRE(g.GetWeights("s1", "s2") == std::vector<double> {5.4});
        REQUIRE(g.IsConnected("s2", "s3"));
        REQUIRE(g.GetWeights("s2", "s3") == std::vector<double> {7.6});
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

      THEN("all nodes are the same") {
        REQUIRE(g.GetNodes() == g2.GetNodes());
      }

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

      THEN("all nodes are the same") {
        REQUIRE(g.GetNodes() == g2.GetNodes());
      }

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

    WHEN("a graph is move assigned from this graph") {
      auto g2 = std::move{g};
      
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

      THEN("the result is true") {
        REQUIRE(b);
      }
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
    }

    WHEN("a duplicate node is inserted") {
      bool b = g.InsertNode("A");

      THEN("the duplicate node is not in the graph") {
        REQUIRE(!b);
        REQUIRE(g.GetNodes() == std::vector<std::string>{"A"});
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
        REQUIRE(g.IsConnected("A", "B"));
        REQUIRE(g.GetWeights("A", "B").size() == 1 && g.GetWeights("B", "A").size() == 0);
      }
    }

    WHEN("a duplicate edge is inserted") {
      g.InsertEdge("A", "B", 2);

      THEN("the edge is not added") {
        REQUIRE(g.GetWeights("A", "B") == std::vector<int>{2});
      }
    }

    WHEN("a different cost is added for an existing edge") {
      g.InsertEdge("A", "B", 4);

      THEN("then a new edge is added") {
        REQUIRE(g.GetWeights("A", "B") == std::vector<int>{2, 4});
      }
    }
  }
}

SCENARIO("Getting Edges") {
  GIVEN("a graph with edges") {
    gdwg::Graph<int, int> g{3, 5, 15};
    g.InsertEdge(3, 5, 9);
    g.InsertEdge(3, 15, -2);

    WHEN("getting the connections of a node with edges") {
      auto connections = g.GetConnections(3);

      THEN("a vector of the nodes neighbours are returned") {
        REQUIRE(connections == std::vector<int>{5, 15});
      }
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
        REQUIRE(connections == std::vector<int>{9});
      }
    }

    WHEN("getting the weights of a multiple connection between 2 nodes (both directed from one node to the other)") {
      g.InsertEdge(3, 5, 999);
      auto weights = g.GetWeights(3, 5);

      THEN("a vector containing all weights of the connection is returned") {
        REQUIRE(connections == std::vector<int>{9, 999});
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

      THEN("all of its outgoing edges are removed") {
        REQUIRE(!g.IsNode("B"));
        REQUIRE(!g.IsConnected("B", "C"));
        REQUIRE(g.GetWeights("B", "C").size() == 0);
      }

      THEN("all of its incoming edges are removed") {
        REQUIRE(!g.IsConnected("A", "B"));
        REQUIRE(g.GetWeights("A", "B").size() == 0);
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
        REQUIRE(g.GetWeights("A", "D") = std::vector<int>{2});
      }

      THEN("the new value has all of old's outgoing edges") {
        REQUIRE(g.IsConnected("D", "C"));
        REQUIRE(g.GetWeights("D", "C") = std::vector<int>{4});
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

      THEN("the replacer node contains all outgoing edges of the replaced node including its own and itself") {
        REQUIRE(!g.IsNode("A"));
        REQUIRE(g.GetConnected("B") == std::vector<std::string>{"B", "C"});
        REQUIRE(g.GetWeights("B", "B") == std::vector<int>{2});
        REQUIRE(g.GetWeights("B", "C") == std::vector<int>{9});
      }

      THEN("the replacer node contains all incoming edges of the replaced node including its own and itself") {
        REQUIRE(g.GetWeights("B", "B") == std::vector<int>{359});
        REQUIRE(g.GetWeights("C", "B") == std::vector<int>{99});
      }
    }

    WHEN("a duplipate edge is formed as a result of merging") {
      g.InsertEdge("B", "B", 2);
      g.MergeReplace("A", "B");

      THEN("the duplicate edge is removed") {
        REQUIRE(g.GetConnected("B") == std::vetor<std::string>{"B", "C"});
        REQUIRE(g.GetWeights("B", "B") == std::vector<int>{2, 9});
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

      THEN("no nodes are in the graph") {
        REQUIRE(g.GetNodes().size() == 0);
      }

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



