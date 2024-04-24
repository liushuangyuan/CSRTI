#ifndef COMMUNITY_H_
#define COMMUNITY_H_
#include "Graph.h"

class Community {
public:
    int id = -1; //Community ID
    set<int> nodeIds;  // The ID of the storage node
    shared_ptr<Community> parent = nullptr;
    vector<shared_ptr<Community>> children;
    shared_ptr<Graph> graphptr = nullptr; 
    int k = 2; //The k-truss where this community is located
    set<shared_ptr<Community>> neighborCommunities;  //Basically useless

    Community() {}
    Community(const set<int> nodeIds, int k) : nodeIds(nodeIds), k(k) {}
    Community(const Graph &graphh, int k);
    Community(const Community& other);
    virtual ~Community();

    // Calculate the sum of degrees of all nodes in the community in G
    int totalDegree(const Graph& G);

    //Obtain the minimum support level for this community edge
    int calculateMinSupport(const Graph &G);

    void printCommunityNodeid();
};

#endif