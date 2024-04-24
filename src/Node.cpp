#include "../include/Node.h"

Node::Node(const Node& other) : id(other.id) {
    // Traverse all edges of the original node, create a new Edge object for each edge, and add it to the current node
    for (const auto& edge : other.edges) {
        edges.push_back(std::make_shared<Edge>(*edge));
    }
}

Node& Node::operator=(const Node& other) {
    if (this == &other) return *this; 
    id = other.id;
    edges.clear();
    for (const auto& edge : other.edges) {
        edges.push_back(std::make_shared<Edge>(*edge));
    }
    return *this;
}