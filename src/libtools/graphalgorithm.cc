/*
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
#include <list>

#include <votca/tools/graph.h>
#include <votca/tools/graph_bf_visitor.h>
#include <votca/tools/graphalgorithm.h>
#include <votca/tools/graphvisitor.h>

using namespace std;

namespace votca {
namespace tools {

/**********************
 * Internal Functions *
 **********************/

list<int> vectorToList_(vector<int> values) {
  list<int> values_list;
  copy(values.begin(), values.end(), back_inserter(values_list));
  return values_list;
}

vector<Edge> edgeSetToVector_(set<Edge> edges) {
  vector<Edge> edges_vector;
  copy(edges.begin(), edges.end(), back_inserter(edges_vector));
  return edges_vector;
}

/********************
 * Public Functions *
 ********************/

bool singleNetwork(Graph g, GraphVisitor& gv) {
  exploreGraph(g, gv);
  return gv.getExploredVertices().size() == g.getVertices().size() &&
         g.getIsolatedNodes().size() == 0;
}

ReducedGraph reduceGraph(Graph g){

  auto tips = g.getVerticesDegree(1);
  auto junctions = g.getJunctions();
  auto vertices = g.getVertices();
  int starting_vertex;
  if(junctions.size()>0){
    starting_vertex = junctions.at(0);
  }else if(tips.size()>0){
    starting_vertex = tips.at(0);
  }else{
    starting_vertex = vertices.at(0);
  }

  vector<vector<int>> chains;
  
  Graph_DF_Visitor gv;
  gv.setStartingVertex(starting_vertex);
  gv.initialize(g);

  vector<int> chain{starting_vertex};
  int old_vertex = starting_vertex;
  bool new_chain = false;
  while (!gv.queEmpty()) {
    auto ed = gv.nextEdge(g);

    auto unexplored_vertex = gv.getUnexploredVertex_(ed);
    if(new_chain){
      if(unexplored_vertex.size()==0){
        old_vertex = ed.getEndPoint1();       
        chain.push_back(old_vertex);
        new_chain = false;
      }else{
        old_vertex = ed.getOtherEndPoint(unexplored_vertex.at(0));
        chain.push_back(old_vertex);
        new_chain = false;
      }
    }
    int new_vertex = ed.getOtherEndPoint(old_vertex);

    if(unexplored_vertex.size()==0){
      chain.push_back(new_vertex);
      chains.push_back(chain);    
      chain.clear();
      new_chain = true; 
    }else if(g.getDegree(new_vertex)==1){
      chain.push_back(new_vertex);
      chains.push_back(chain);
      chain.clear();
      new_chain = true; 
    }else if(unexplored_vertex.size()==1){
      chain.push_back(new_vertex); 
      old_vertex = new_vertex;
    }

    gv.exec(g, ed);
  }

}

vector<shared_ptr<Graph>> decoupleIsolatedSubGraphs(Graph g) {

  auto vertices_list = vectorToList_(g.getVertices());
  vector<shared_ptr<Graph>> subGraphs;
  {
    Graph_BF_Visitor gv_breadth_first;
    gv_breadth_first.setStartingVertex(*vertices_list.begin());
    if (singleNetwork(g, gv_breadth_first)) {
      subGraphs.push_back(make_shared<Graph>(g));
      return subGraphs;
    }
  }

  auto v_it = vertices_list.begin();
  unordered_map<int, GraphNode> sub_graph_nodes;
  while (v_it != vertices_list.end()) {
    Graph_BF_Visitor gv_breadth_first;
    gv_breadth_first.setStartingVertex(*v_it);
    exploreGraph(g, gv_breadth_first);

    ++v_it;
    auto sub_graph_explored_vertices = gv_breadth_first.getExploredVertices();
    set<Edge> sub_graph_edges;

    auto sub_graph_vertex_it = sub_graph_explored_vertices.begin();
    while(sub_graph_vertex_it!=sub_graph_explored_vertices.end()) {
      if (*v_it == *sub_graph_vertex_it) {
        ++v_it;
      }
      vertices_list.remove(*sub_graph_vertex_it);

      auto sub_graph_neigh_edges = g.getNeighEdges(*sub_graph_vertex_it);
      for (auto sub_graph_edge : sub_graph_neigh_edges) {
        sub_graph_edges.insert(sub_graph_edge);
      }

      sub_graph_nodes[*sub_graph_vertex_it] = g.getNode(*sub_graph_vertex_it);
      ++sub_graph_vertex_it;
    }

    auto sub_graph_vector_edges = edgeSetToVector_(sub_graph_edges);

    subGraphs.push_back(
        make_shared<Graph>(Graph(sub_graph_vector_edges, sub_graph_nodes)));
  }
  return subGraphs;
}

void exploreGraph(Graph& g, GraphVisitor& gv) {

  if (!g.vertexExist(gv.getStartingVertex())) {
    string err = "Cannot explore graph starting at vertex " +
                 to_string(gv.getStartingVertex()) +
                 " because it does not exist in the "
                 "graph. You can change the starting vertex by using the "
                 "setStartingVertex method of the visitor instance.";
    throw invalid_argument(err);
  }
  // Create a list of all vertices and determine if they have all been
  // explored when the visitor que is empty
  gv.initialize(g);
  while (!gv.queEmpty()) {
    auto ed = gv.nextEdge(g);
    gv.exec(g, ed);
  }
}
}
}
