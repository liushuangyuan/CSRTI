#ifndef GRAPH_H_
#define GRAPH_H_
#include "Node.h"

class Graph {
public:
    map<int, shared_ptr<Node>> nodes; // Store the mapping of all nodes in the graph
    int numEdges; // The total number of edges in the graph
    static bool dynamicUpdateIsOrNot;

    Graph() : numEdges(0) {} 

    // Traverse and output all nodes and edges in the graph
    void printGraph() const;

    Graph(const Graph& other);

    // Construct a new graph based on the existing graph G and node set
    Graph(const Graph& G, const set<int>& nodeSet);

    Graph& operator=(const Graph& other);

    shared_ptr<Edge> addEdge(int from, int to, int weight = 1);

    // Load or Build Graph
    void loadGraph(const string& filePath);

    // Obtain the IDs of all nodes in the graph
    set<int> getAllNodeIds() const;
    //Return the set of all edges in this graph
    set<shared_ptr<Edge>> getEdges();

    //Return the pairs of edges that form triangles with curE in this graph.
    vector<pair<shared_ptr<Edge>, shared_ptr<Edge>>> findTriangleEdges(const shared_ptr<Edge>& curE);

    // Obtain the support of all edges and return the minimum support of edges
    int getAllEdgeSupports();

    //
    void removeEdge(int from, int to);
    void removeNode(int removeNode);

    // Connected component
    set<set<int>> getAllConnectedComponents(bool OneNodeIsCommunity = false);
    // Delete the edge in the graph and update the support of other edges that can form a triangle with it
    bool removeEdgeAndUpdateSupports(shared_ptr<Edge> edge);

    // Edge support update
    bool UpdateSupports(shared_ptr<Edge> edge);

    void printEdgesForNodeset(const set<int>& nodeids);
    set<int> getNumNodes(int num, vector<int> allNodes);

    // Operations related to dynamic files
    void writeDynamicDataToFile(const string &filename, pair<int, int> addOrRemoveEdge, bool addEdgeOrContrary);
    void writeDynamicDataToFile(const string &filename, int node_id, bool addNodeOrContrary, set<int> neiIds);
    void readDynamicDataAndUpdate(const string filePath);
private:
    bool edgeExists(const shared_ptr<Edge>& edge);
};


#endif