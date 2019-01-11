/*
 *            E
 *            Copyright 2009-2018 The VOTCA Development Team
 *                       (http://www.votca.org)
 *
 *      Licensed under the Apache License, Version 2.0 (the "License")
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <votca/tools/graph.h>
#include <votca/tools/reducededge.h>

#ifndef _VOTCA_TOOLS_REDUCEDGRAPH_H
#define _VOTCA_TOOLS_REDUCEDGRAPH_H

namespace votca {
namespace tools {

/**
 * \brief Contains a graph that consits of vertices with degree of 1 or greater than 3
 *
 * The point of this class is to reduce the computational complexity of a
 * regular graph. This is achieved by removing any vertices with degree 2. For 
 * example a graph:
 *
 * 1 - 2 - 3 - 4 - 5 - 9
 *     |   |   |
 *     6 - 7   8
 *
 * Would be reduced to 
 *
 * 1 - 2 - 3 - 4 - 9
 *     | _ |   |
 *             8
 *
 * Notice that the vertices 5, 6 and 7 have been removed, there also exist two
 * edges connecting 2 to 3. The reduced graph still contains all the information
 * associated with the full graph but when used with graph algorithms only the
 * vertices and nodes associated with the reduced graph are used.  
 *
 **/
class ReducedGraph : public Graph {
 private:

   EdgeContainer edge_container_full_;
   std::unordered_map<Edge,std::vector<std::vector<int>>> expanded_edges_;  

    void initEdgeContainerFull_(std::vector<ReducedEdge> reduced_edges);

    void init_(std::vector<ReducedEdge> reduced_edges, std::unordered_map<int,GraphNode> nodes);

 public:
  ReducedGraph(){};
  ~ReducedGraph(){};

  ReducedGraph(std::vector<ReducedEdge> reduced_edges);
  ReducedGraph(std::vector<ReducedEdge> reduced_edges, std::unordered_map<int,GraphNode> nodes);

  ReducedGraph(const ReducedGraph & reduced_graph);

  ReducedGraph& operator=(const ReducedGraph& reduced_graph);

  ReducedGraph& operator=(ReducedGraph&& reduced_graph);

  /**
   * \brief Allows one to return all edges connecting two vertices of the reduced
   * graph.
   *
   * In this case if edge (2,3) were passed in:
   *
   * 1 - 2 - 3 - 4 - 5 - 9
   *     |   |   |
   *     6 - 7   8
   *
   * Reduced Graph
   *
   * 1 - 2 - 3 - 4 - 9
   *     | _ |   |
   *             8
   *
   * The following vectors would be returned
   * 
   * vec_edges = expandEdge(Edge(2,3));
   * vec_edges.at(0); // 2-3
   * vec_edges.at(1); // 2-6, 6-7, 7-3
   **/
  std::vector<std::vector<Edge>> expandEdge(Edge ed);

  std::vector<std::pair<int, GraphNode>> getNodes(void);

  friend std::ostream& operator<<(std::ostream& os, const ReducedGraph g);
};

}
}
#endif  // _VOTCA_TOOLS_REDUCEDGRAPH_H
