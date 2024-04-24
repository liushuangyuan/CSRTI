#include "../include/Community.h"

Community::Community(const Graph &graphh, int k) : k(k) {
    nodeIds = graphh.getAllNodeIds();
    graphptr = make_shared<Graph>(graphh);
}

void Community::printCommunityNodeid() {
    cout << "Community nodes: ";
    for (auto x : nodeIds) {
        cout << x << " ";
    }
    cout << ", Community size: " << nodeIds.size() << endl;
    if (graphptr == nullptr) return ;
    for (auto &node : graphptr->nodes) {
        for (auto &edge : node.second->edges) {
            if (max(edge->from, edge->to) == node.first) continue;
            cout << edge->from << " " << edge->to << endl;
        }
    }
    cout << "The number of edges in the community: " << graphptr->numEdges << endl;
} 

Community::Community(const Community& other) {
    nodeIds = other.nodeIds;
    k = other.k;
    if (other.graphptr) {
        graphptr = make_shared<Graph>(*other.graphptr);
    } else {
        graphptr = nullptr; 
    }  
    neighborCommunities.clear();
}

Community::~Community() {
    nodeIds.clear();
    parent = nullptr;
    children.clear();
    neighborCommunities.clear();
}

int Community::totalDegree(const Graph& G) {
    int sumDegree = 0;

    for (int nodeId : nodeIds) {
        auto nodeIt = G.nodes.find(nodeId);
        if (nodeIt != G.nodes.end()) {
            sumDegree += nodeIt->second->edges.size();
        }
    }
    return sumDegree;
}

int Community::calculateMinSupport(const Graph &G) { 
    if (graphptr == nullptr) graphptr = make_shared<Graph>(G, nodeIds);
    int minSupport = graphptr->getAllEdgeSupports();
    return minSupport;
}

