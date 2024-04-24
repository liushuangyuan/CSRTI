#ifndef NODE_H_
#define NODE_H_
#include "common.h"
#include "Edge.h"
class Node {
public:
    int id = -1; // identifier 
    vector<shared_ptr<Edge>> edges; // List of edges connected to this node.

    Node(int id) : id(id) {} 

    Node(const Node& other);

    Node& operator=(const Node& other);
};

#endif