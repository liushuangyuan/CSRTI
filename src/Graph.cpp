#include "../include/Graph.h"

bool Graph::dynamicUpdateIsOrNot = false;

void Graph::printGraph() const {
    cout << "Graph contains " << nodes.size() << " nodes and " << numEdges << " edges." << endl;
    for (const auto& pair : nodes) {
        int nodeId = pair.first;
        const auto& node = pair.second;
        cout << "Node " << nodeId << " connects to: ";
        for (const auto& edge : node->edges) {
            cout << "(" << edge->to << ", weight=" << edge->weight << ") ";
        }
        cout << endl;
    }
}

Graph::Graph(const Graph& other) : numEdges(other.numEdges) {

    for (const auto& nodePair : other.nodes) {
        auto newNode = make_shared<Node>(nodePair.first);
        nodes[nodePair.first] = newNode;
    }

    // Traverse all nodes in the original graph
    map<pair<int, int>, shared_ptr<Edge>> edgeSet; 
    for (const auto& nodePair : other.nodes) {
        auto &newNode = nodes[nodePair.first]; 
        for (auto edge : nodePair.second->edges) {
            pair<int, int> edgePair(min(edge->from, edge->to), max(edge->from, edge->to));
            if (edgeSet.find(edgePair) == edgeSet.end()) {

                auto newEdge = make_shared<Edge>(min(edge->from, edge->to),max(edge->from, edge->to), edge->weight, edge->support);
                newNode->edges.push_back(newEdge); 
                edgeSet[edgePair] = newEdge; 
            } else {
                newNode->edges.push_back(edgeSet.find(edgePair)->second); 
            }
        }
    }
}

Graph::Graph(const Graph& G, const set<int>& nodeSet) : numEdges(0) {
    //Create new nodes
    for (const auto& nodeid : nodeSet) {
        auto newNode = make_shared<Node>(nodeid);
        nodes[nodeid] = newNode;
    }

    map<pair<int, int>, shared_ptr<Edge>> edgeSet; 
    for (int nodeid : nodeSet) {
        if (G.nodes.find(nodeid) == G.nodes.end()) continue; 
        //Traverse each edge of this node in the original graph
        for (auto &edge : G.nodes.at(nodeid)->edges) {
            int from = min(edge->from, edge->to);
            int to = max(edge->to, edge->from);
            
            if (nodeSet.find(from) == nodeSet.end() || nodeSet.find(to) == nodeSet.end()) continue;
            pair<int, int> edgePair(from, to);
            
            if (edgeSet.find(edgePair) == edgeSet.end()) {
                
                auto newEdge = make_shared<Edge>(from, to, edge->weight, edge->support);
                nodes[nodeid]->edges.push_back(newEdge); 
                edgeSet[edgePair] = newEdge; 
                numEdges++; 
            } else {
                nodes[nodeid]->edges.push_back(edgeSet.find(edgePair)->second); 
            }

        }
    }
}

Graph& Graph::operator=(const Graph& other) {
    if (this == &other) return *this; 
    nodes.clear();
    numEdges = other.numEdges;
    for (const auto& nodePair : other.nodes) {
        auto newNode = make_shared<Node>(nodePair.first);
        nodes[nodePair.first] = newNode;
    }

    map<pair<int, int>, shared_ptr<Edge>> edgeSet; 
    for (const auto& nodePair : other.nodes) {
        auto &newNode = nodes[nodePair.first]; 
        for (auto edge : nodePair.second->edges) {
            pair<int, int> edgePair(min(edge->from, edge->to), max(edge->from, edge->to));
            if (edgeSet.find(edgePair) == edgeSet.end()) {
                auto newEdge = make_shared<Edge>(min(edge->from, edge->to),max(edge->from, edge->to), edge->weight, edge->support);
                newNode->edges.push_back(newEdge); 
                edgeSet[edgePair] = newEdge; 
            } else {
                newNode->edges.push_back(edgeSet.find(edgePair)->second); 
            }
        }
    }
    return *this;
}

shared_ptr<Edge> Graph::addEdge(int from, int to, int weight) {
   
    if (nodes.find(from) == nodes.end()) {
        nodes[from] = make_shared<Node>(from);
    }
    if (nodes.find(to) == nodes.end()) {
        nodes[to] = make_shared<Node>(to);
    }
    for (auto &edge : nodes.find(from)->second->edges) {
        if ((edge->from == from && edge->to == to) || (edge->from == to && edge->to == from)) return nullptr;
    }
    
    auto newEdge = make_shared<Edge>(min(from, to), max(from, to), weight, 0);
 
    nodes[from]->edges.push_back(newEdge);
    nodes[to]->edges.push_back(newEdge); 

    numEdges++;
    return newEdge;
}

void Graph::loadGraph(const string& filePath) {
    ifstream file(filePath);
    string line;

    if (!file.is_open()) {
        cerr << "Error opening file: " << filePath << endl;
        return;
    }

    while (getline(file, line)) {
        istringstream iss(line);
        int from, to, weight = 1;
    
        if (!(iss >> from >> to)) {
            cerr << "Parsing error: " << line << endl;
            continue; 
        }
        iss >> weight;
        this->addEdge(from, to, weight);
    }
    cout << "File read completed." << endl;
    file.close();
}

void Graph::printEdgesForNodeset(const set<int>& nodeids) {
    if (!this) return ;
    cout << "Community nodes: ";
    for (auto x : nodeids) {
        cout << x << " ";
    }
    cout << ", Community size" << nodeids.size() << endl;
    int edgesize = 0;
    for (auto &node : nodeids) {
        for (auto &edge : nodes[node]->edges) {
            if (max(edge->from, edge->to) == node) continue;
            if (nodeids.find(max(edge->from, edge->to)) == nodeids.end()) continue;
            edgesize++;
            cout << edge->from << " " << edge->to << endl;
        }
    }
    
    cout << "The number of edges in the community: " << edgesize << endl;
}

set<int> Graph::getAllNodeIds() const {
    set<int> nodeIds;
    for (const auto& nodePair : nodes) {
        nodeIds.insert(nodePair.first);
    }
    return nodeIds;
}


vector<pair<shared_ptr<Edge>, shared_ptr<Edge>>> Graph::findTriangleEdges(const shared_ptr<Edge>& curE) {
    vector<pair<shared_ptr<Edge>, shared_ptr<Edge>>> result;

    int from = curE->from;
    int to = curE->to;

    set<shared_ptr<Edge>> A1, A2;
    if (nodes.count(from)) {
        for (auto& edge : nodes[from]->edges) {
            if (edge != curE) {
                A1.insert(edge);
            }
        }
    }
    if (nodes.count(to)) {
        for (auto& edge : nodes[to]->edges) {
            if (edge != curE) {
                A2.insert(edge);
            }
        }
    }

    for (auto& edge1 : A1) {
        int other1 = (edge1->from == from) ? edge1->to : edge1->from;
        auto nodeIt = nodes.find(other1);
        if (nodeIt != nodes.end()) {
            for (auto& edge2 : nodeIt->second->edges) {
                if (edge2 == edge1) continue;
                if (A2.find(edge2) != A2.end() && edge2 != curE) {
                    result.push_back(make_pair(edge1, edge2));
                    break;
                }
            }
        }
    }

    return result;
}

set<shared_ptr<Edge>> Graph::getEdges() {
    set<shared_ptr<Edge>> edgesInCommunity; 
    if (this == nullptr) return edgesInCommunity;
    for (auto &nodes : this->nodes) {
        for (auto &edge : nodes.second->edges) {
            edgesInCommunity.insert(edge);
        }
    }
    return edgesInCommunity;
}

int Graph::getAllEdgeSupports() {

    auto edges = getEdges();
    int minSupport = numeric_limits<int>::max(); //The support of the minimum edge

    for (auto& curE : edges) {

        auto triangleEdges = findTriangleEdges(curE);
        // Support is the number of side pairs that make up a triangle
        curE->support = triangleEdges.size();
        if (curE->support < minSupport) minSupport = curE->support;
    }
    if (minSupport == numeric_limits<int>::max()) minSupport = 0;
    return minSupport;
}

void Graph::removeNode(int removeNode) {
    if (nodes.find(removeNode) == nodes.end()) return ;
    vector<shared_ptr<Edge>> edgesCopy = nodes[removeNode]->edges;
    for (auto edgel : edgesCopy) {
        removeEdge(edgel->from, edgel->to);
    }
    nodes.erase(removeNode);
    return ;
}

void Graph::removeEdge(int from, int to) {
    bool flag = true;
    if (nodes.count(from)) { 
        auto& edgesFrom = nodes[from]->edges;
        edgesFrom.erase(
            remove_if(edgesFrom.begin(), edgesFrom.end(), [from, to](const shared_ptr<Edge>& edge) {
                return (edge->from == from && edge->to == to) || (edge->to == from && edge->from == to);
            }),
            edgesFrom.end()
        );
    } else flag = false;
    if (!flag) return ;

    if (nodes.count(to)) { 
        auto& edgesTo = nodes[to]->edges;
        edgesTo.erase(
            remove_if(edgesTo.begin(), edgesTo.end(), [from, to](const shared_ptr<Edge>& edge) {
                return (edge->from == from && edge->to == to) || (edge->to == from && edge->from == to);
            }),
            edgesTo.end()
        );
    } else flag = false;
    if (!flag) return ;

    numEdges--;
    return ;
}

set<set<int>> Graph::getAllConnectedComponents(bool OneNodeIsCommunity) {
    set<int> nodeIdSet; 
    for (const auto& nodePair : nodes) {
        nodeIdSet.insert(nodePair.first);
    }

    set<set<int>> connectedComponents; 

    while (!nodeIdSet.empty()) {
        int startNodeId = *nodeIdSet.begin(); 
        nodeIdSet.erase(startNodeId); 

        set<int> currentComponent; 
        queue<int> toVisit; 
        toVisit.push(startNodeId);

        while (!toVisit.empty()) {
            int nodeId = toVisit.front();
            toVisit.pop();
            currentComponent.insert(nodeId); // Add the current node to the connected component

            for (const auto& edge : nodes[nodeId]->edges) {
                int neighborId = (edge->from == nodeId) ? edge->to : edge->from;
                if (nodeIdSet.find(neighborId) != nodeIdSet.end()) {
                    toVisit.push(neighborId);
                    nodeIdSet.erase(neighborId);
                }
            }
        }
        if (currentComponent.empty()) continue;
        if (!OneNodeIsCommunity && currentComponent.size() == 1) continue; 
        connectedComponents.insert(currentComponent); 
    }
    
    return connectedComponents;
}

bool Graph::removeEdgeAndUpdateSupports(shared_ptr<Edge> edge) {
    if (!edgeExists(edge)) {
        cout << "Edge does not exist." << endl;
        return false;
    }

    int from = edge->from;
    int to = edge->to;

    // Find all edges that can form a triangle with edge and update their support level
    auto triangleEdges = findTriangleEdges(edge);
    for (auto& triangleEdgePair : triangleEdges) {
        auto edge1 = triangleEdgePair.first;
        auto edge2 = triangleEdgePair.second;
        if (edge1->support > 0) (edge1->support)--;
        if (edge2->support > 0) (edge2->support)--;
    }

    removeEdge(from, to);
    return true;
}

bool Graph::UpdateSupports(shared_ptr<Edge> edge) {
    if (!edgeExists(edge)) {
        cout << "Edge does not exist." << endl;
        return false;
    }

    int from = edge->from;
    int to = edge->to;

    auto triangleEdges = findTriangleEdges(edge);
    for (auto& triangleEdgePair : triangleEdges) {
        auto edge1 = triangleEdgePair.first;
        auto edge2 = triangleEdgePair.second;
        ++edge1->support;
        ++edge2->support;
    }
    return true;
}

bool Graph::edgeExists(const shared_ptr<Edge>& edge) {
    int from = edge->from, to = edge->to;
    if (nodes.find(from) == nodes.end() || nodes.find(to) == nodes.end()) {
        return false; 
    }

    // Check if the edge from to exists
    const auto& edgesFrom = nodes[from]->edges;
    bool existsInFrom = any_of(edgesFrom.begin(), edgesFrom.end(), [from, to](const shared_ptr<Edge>& e) {
        return (e->from == from && e->to == to) || (e->from == to && e->to == from);
    });

    const auto& edgesTo = nodes[to]->edges;
    bool existsInTo = any_of(edgesTo.begin(), edgesTo.end(), [from, to](const shared_ptr<Edge>& e) {
        return (e->from == from && e->to == to) || (e->from == to && e->to == from);
    });

    return existsInTo && existsInFrom;
}

set<int> Graph::getNumNodes(int num, vector<int> allNodes) {
    set<int> visit;
    if (nodes.empty()) return visit;
    num = (num > nodes.size() ? nodes.size() : num);

    queue<int> que;

    que.push(allNodes.back());
    visit.insert(allNodes.back());
    allNodes.pop_back();

    while (!que.empty()) {
        if (visit.size() >= num) break;
        int curNodeid = que.front();
        que.pop();
        
        for (auto edge : nodes[curNodeid]->edges) {
            int to = (edge->from == curNodeid ? edge->to : edge->from); 
            if (visit.find(to) != visit.end()) continue;
            if (visit.size() >= num) break;
            visit.insert(to);
            allNodes.erase(find(allNodes.begin(), allNodes.end(), to));
            que.push(to);
        }
        
        if (que.empty() && (visit.size() < num)) {
            que.push(allNodes.back());
            visit.insert(allNodes.back());
            allNodes.pop_back();
        }
    }
    cout << "Number of nodes obtained: " << visit.size() << endl;

    return visit;
}

void Graph::writeDynamicDataToFile(const string &filename, pair<int, int> addOrRemoveEdge, bool addEdgeOrContrary) {
    ofstream outFile(filename, ios::app); 

    if (!outFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    if (addEdgeOrContrary) outFile << "AddEdge: ";
    else outFile << "RemoveEdge: ";
    int from = min(addOrRemoveEdge.first, addOrRemoveEdge.second);
    int to = max(addOrRemoveEdge.first, addOrRemoveEdge.second);
    outFile << from << " " << to << endl;
    outFile.close();
}

void Graph::writeDynamicDataToFile(const string &filename, int node_id, bool addNodeOrContrary, set<int> neiIds) {

    ofstream outFile(filename, ios::app); 
    if (!outFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    if (addNodeOrContrary) outFile << "AddNode: ";
    else outFile << "RemoveNode: ";
    outFile << node_id << " ";
    if(addNodeOrContrary) for (auto nei : neiIds) outFile << nei << " ";
    outFile << endl;
    outFile.close();
}

void Graph::readDynamicDataAndUpdate(const string filePath) {
    if (Graph::dynamicUpdateIsOrNot) return ; 

    if (!filesystem::exists(filePath)) { 
        Graph::dynamicUpdateIsOrNot = true;
        return ;
    }

    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filePath << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string command;
        iss >> command;
        if (command.back() == ':') {
            command.pop_back();
        } else continue;
        
        if (command == "AddEdge") {
            int from, to;
            iss >> from >> to;
            addEdge(from, to);
        } else if (command == "RemoveEdge") {
            int from, to;
            iss >> from >> to;
            removeEdge(from, to);
        } else if (command == "AddNode") {
            int AddnodeId;
            iss >> AddnodeId;
            int node_id;
            while (iss >> node_id) {
                addEdge(AddnodeId, node_id);
            }
        } else if (command == "RemoveNode") {
            int RemovenodeId;
            iss >> RemovenodeId;
            removeNode(RemovenodeId);
        }
    }

    Graph::dynamicUpdateIsOrNot = true;
    file.close();
    return ;
}