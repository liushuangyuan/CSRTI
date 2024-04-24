#include "../include/CSKNI.h"

void CSKNI::constructIndexTreeAndUpdateMap(set<shared_ptr<Community>>& BCSet) {
    int k = 3;
    for (auto &curC : BCSet) {
        curC->graphptr->getAllEdgeSupports();
    }
    cout << "Calculation of support for all edges is complete." << endl;
    // Truss generation and map linking.
    for (auto &curC : BCSet) {
        GetTrussCommunitiesAndUpdateMap(nodeCommunityMap, k, curC);
    }
}

void CSKNI::GetTrussCommunitiesAndUpdateMap(map<int, shared_ptr<Community>>& nodeToCommunityMap, int k, const shared_ptr<Community> &C) {
    set<shared_ptr<Community>> CSet; 
    if (!C->graphptr) return;       
    shared_ptr<Graph> newGraph = make_shared<Graph>(*(C->graphptr)); 
    C->graphptr = nullptr;
    auto edgesList = newGraph->getEdges();
    vector<shared_ptr<Edge>> sortedEdgesList(edgesList.begin(), edgesList.end());
    bool removed;
    do {
        if (sortedEdgesList.empty()) break;
        removed = false;
        // Sort EdgesList by edge support from smallest to largest.
        sort(sortedEdgesList.begin(), sortedEdgesList.end(), 
            [](const shared_ptr<Edge>& a, const shared_ptr<Edge>& b) -> bool {
                return a->support < b->support;
            });
        set<shared_ptr<Edge>> deleteEdge; 
        for (auto &edge : sortedEdgesList) {
            if (edge->support >= k - 2) break;
            deleteEdge.insert(edge); 
        }
        cout << "Number of edges to be deleted during truss decomposition:" << deleteEdge.size() << ", Number of remaining edges: " << sortedEdgesList.size() - deleteEdge.size() << endl; 
        if (deleteEdge.empty()) break;
        else removed = true;
        for (auto &edge : deleteEdge) {
            auto findEdge = find(sortedEdgesList.begin(), sortedEdgesList.end(), edge);
            if (findEdge == sortedEdgesList.end()) {
                continue;
            }
            sortedEdgesList.erase(findEdge); 
            newGraph->removeEdgeAndUpdateSupports(edge);
        }
        cout << "One iteration complete." << endl;
    } while (removed); 

    // Check all remaining connected nodes in C as new communities and add to CSet.
    auto connectedComponents = newGraph->getAllConnectedComponents();

    C->children.clear(); 
    for (const auto& component : connectedComponents) {
        auto newCommunity = make_shared<Community>(component, k); 
        newCommunity->graphptr = make_shared<Graph>(*newGraph, component); 
        newCommunity->parent = C;
        C->children.push_back(newCommunity);
        for (int nodeId : newCommunity->nodeIds) {
            nodeToCommunityMap[nodeId] = newCommunity;
        }
        CSet.insert(newCommunity);
    }


    for (auto& curC : CSet) {
        GetTrussCommunitiesAndUpdateMap(nodeToCommunityMap, k + 1, curC);
    }
    return ;
}

set<int> CSKNI::NCS(const set<int>& Q, int k) {

    set<int> resultNode;
    if (k < 2) return resultNode;
    shared_ptr<Community> CQ = nullptr;
    vector<shared_ptr<Community>> CSet;
    for (int queryNodeId : Q) {
        auto currentCommunity = nodeCommunityMap[queryNodeId];

        if (currentCommunity && currentCommunity->k >= k) {
            if (find(CSet.begin(), CSet.end(), currentCommunity) == CSet.end()) CSet.push_back(currentCommunity);
        } else {
            return resultNode;
        }
    }

    CQ = findCommonAncestor(CSet);
    if (CQ == nullptr || CQ->k < k) return resultNode;
    while (CQ->k > k) {
        if (CQ->parent && CQ->parent->k < k) break;
        CQ = CQ->parent;
    }
    for (int nodeidd : CQ->nodeIds) {
        resultNode.insert(nodeidd);
    }
    return resultNode;
}

void CSKNI::writeSearchResultsToFile(const string& filename, const set<int>& communityNodes, const set<int>& queryNodes, int k) {
    vector<int> result;
    for (auto &nodeid : communityNodes) {
        result.push_back(nodeid);
    }
    sort(result.begin(), result.end());
    ofstream outFile(filename, ios::app);

    if (!outFile.is_open()) {
        cerr << "Unable to open file: " << filename << endl;
        return;
    }

    outFile << "k: " << k << endl;

    vector<int> query;
    for (auto &nodeid : queryNodes) {
        query.push_back(nodeid);
    }
    sort(query.begin(), query.end());
    outFile << "Query Nodes: ";
    for (size_t i = 0; i < query.size(); ++i) {
        outFile << query[i] << " ";
    }
    outFile << endl;

    outFile << "Result Nodes: ";
    for (size_t i = 0; i < result.size(); ++i) {
        outFile << result[i] << " ";
    }
    outFile << endl << endl;
    outFile.close();
}

void CSKNI::IndexRemoveNode(int RemoveNode, string IndexFilePath, string DynamicFilePath) {
    initGraph.readDynamicDataAndUpdate(DynamicFilePath);
    if (initGraph.nodes.find(RemoveNode) == initGraph.nodes.end()) return ;
    initGraph.removeNode(RemoveNode);
    initGraph.writeDynamicDataToFile(DynamicFilePath, RemoveNode, false, set<int>());

    auto curCom = nodeCommunityMap[RemoveNode];
    if (curCom->k < 2) {
        nodeCommunityMap.erase(RemoveNode);
        return ;
    }

    while (curCom->k > 2) {
        if (curCom->parent && curCom->parent->k < 2) break;
        curCom = curCom->parent;
    }

    // Remove nodes from the basic community node set.
    auto it1 = curCom->nodeIds.find(RemoveNode);
    if (it1 != curCom->nodeIds.end()) curCom->nodeIds.erase(it1);
    nodeCommunityMap.erase(RemoveNode);

    
    shared_ptr<Graph> newGraph =  make_shared<Graph>(initGraph, curCom->nodeIds);

    auto connectedComponents = newGraph->getAllConnectedComponents(true);
    set<shared_ptr<Community>> CSet;
    for (const auto& component : connectedComponents) {
        auto newCommunity = make_shared<Community>(component, 2); 
        newCommunity->graphptr = make_shared<Graph>(*newGraph, component); 
        newCommunity->parent = initCommunity;
        initCommunity->children.push_back(newCommunity);
        for (int nodeId : newCommunity->nodeIds) {
            nodeCommunityMap[nodeId] = newCommunity;
        }
        CSet.insert(newCommunity);
    }

    auto it = find(initCommunity->children.begin(), initCommunity->children.end(), curCom);
    if (it !=  initCommunity->children.end()) {
        (*it)->parent = nullptr;
        initCommunity->children.erase(it);
    }

    for (auto& curC : CSet) {
        curC->graphptr->getAllEdgeSupports(); 
        GetTrussCommunitiesAndUpdateMap(nodeCommunityMap, 3, curC);
    }
    
    assignCommunityIds(initCommunity);
    writeCommunityMapToFile(IndexFilePath);
}

void CSKNI::IndexRemoveEdge(int from, int to, string IndexFilePath, string DynamicFilePath) {
    initGraph.readDynamicDataAndUpdate(DynamicFilePath);
    if (initGraph.nodes.find(from) == initGraph.nodes.end() || initGraph.nodes.find(to) == initGraph.nodes.end()) return ;

    bool edgeExistence = false; // Does the edge to be deleted exist
    for (auto edge : initGraph.nodes[from]->edges) {
        int edgeto = (edge->from == from ? edge->to : edge->from);
        if (edgeto == to) {
            edgeExistence = true;
            break;
        }
    }
    if (!edgeExistence) return ;
    initGraph.removeEdge(from, to); 
    // Write operations to the dynamic file.
    initGraph.writeDynamicDataToFile(DynamicFilePath, pair<int, int>(from, to), false);

    if (nodeCommunityMap[from]->k <= 1 || nodeCommunityMap[to]->k <= 1) {
        // The first level index is almost useless, so there's no need to change it.
        return ;
    }
    shared_ptr<Community> c1 = nodeCommunityMap[from];
    shared_ptr<Community> c2 = nodeCommunityMap[to];
    
    while (c1->k > 2) {
        if (c1->parent && c1->parent->k < 2) break;
        c1 = c1->parent;
    }
    while (c2->k > 2) {
        if (c2->parent && c2->parent->k < 2) break;
        c2 = c2->parent;
    }
    if (c1 != c2) return ;
  
    shared_ptr<Graph> newGraph =  make_shared<Graph>(initGraph, c1->nodeIds);

    auto connectedComponents = newGraph->getAllConnectedComponents(true);
    set<shared_ptr<Community>> CSet;
    for (const auto& component : connectedComponents) {
        auto newCommunity = make_shared<Community>(component, 2); 
        newCommunity->graphptr = make_shared<Graph>(*newGraph, component); 
        newCommunity->parent = initCommunity;
        initCommunity->children.push_back(newCommunity);
        for (int nodeId : newCommunity->nodeIds) {
            nodeCommunityMap[nodeId] = newCommunity;
        }
        CSet.insert(newCommunity);
    }

    auto it = find(initCommunity->children.begin(), initCommunity->children.end(), c1);
    if (it !=  initCommunity->children.end()) {
        (*it)->parent = nullptr;
        initCommunity->children.erase(it);
    }

    for (auto& curC : CSet) {
        curC->graphptr->getAllEdgeSupports(); 
        GetTrussCommunitiesAndUpdateMap(nodeCommunityMap, 3, curC);
    }
    
    assignCommunityIds(initCommunity);
    writeCommunityMapToFile(IndexFilePath);
}

void CSKNI::IndexAddNode(int addNodeId, set<int> neiNodeId, string IndexFilePath, string DynamicFilePath) {
    initGraph.readDynamicDataAndUpdate(DynamicFilePath);
    if (initGraph.nodes.find(addNodeId) != initGraph.nodes.end()) {
        cout << "The node already exists, no need to add." << endl;
        return ;
    }
    for (auto nei : neiNodeId) {
        if (initGraph.nodes.find(nei) == initGraph.nodes.end()) return ;
    }
    
    for (auto to : neiNodeId) initGraph.addEdge(addNodeId, to);
    initGraph.writeDynamicDataToFile(DynamicFilePath, addNodeId, true, neiNodeId);
    
    set<shared_ptr<Community>> communitySet;
    for (auto to : neiNodeId) {
        if (nodeCommunityMap[to]->k <= 1) {
            replaceRelaxedConstructIndex(initGraph, IndexFilePath, "LPA", true);
            return ;
        }
        communitySet.insert(nodeCommunityMap[to]);
    }

    set<int> community;
    community.insert(addNodeId);
    shared_ptr<Community> curCommunity = make_shared<Community>(); 

    initCommunity->children.push_back(curCommunity);
    curCommunity->parent = initCommunity;
    nodeCommunityMap[addNodeId] = curCommunity; 
    curCommunity->nodeIds.insert(addNodeId); 
    curCommunity->k = 2;

    set<shared_ptr<Community>> communitySet1;

    for (auto &com : communitySet) {
        auto curcom = com;
        while (curcom->k > 2) {
            if (curcom->parent && curcom->parent->k < 2) break;
            if (!curcom->parent) {
                replaceRelaxedConstructIndex(initGraph, IndexFilePath, "LPA", true);
                return ;
            }
            curcom = curcom->parent;
        }
        communitySet1.insert(curcom);
    }

    for (auto &com : communitySet1) {
        if (com != curCommunity) {
            deepMergeCommunities(curCommunity, com, nodeCommunityMap); 

            auto it = find(initCommunity->children.begin(), initCommunity->children.end(), com);
            if (it != initCommunity->children.end()) {
                (*it)->parent = nullptr;
                initCommunity->children.erase(it);
            }
        }
    }

    curCommunity->graphptr = make_shared<Graph>(initGraph, curCommunity->nodeIds);
    curCommunity->graphptr->getAllEdgeSupports(); 
    GetTrussCommunitiesAndUpdateMap(nodeCommunityMap, 3, curCommunity);

    assignCommunityIds(initCommunity);
    writeCommunityMapToFile(IndexFilePath);

    return ;
}
 
void CSKNI::IndexAddEdge(int from, int to, string IndexFilePath, string DynamicFilePath) {
    initGraph.readDynamicDataAndUpdate(DynamicFilePath);
    bool constructIndexAgain = false;
    if (initGraph.nodes.find(from) == initGraph.nodes.end() || initGraph.nodes.find(to) == initGraph.nodes.end()) constructIndexAgain = true;
    
    auto newEdge = initGraph.addEdge(from, to);
    // If the edge exists.
    if (!newEdge) return ;
    initGraph.writeDynamicDataToFile(DynamicFilePath, pair<int, int>(from, to), true);

    if (constructIndexAgain || nodeCommunityMap[from]->k <= 1 || nodeCommunityMap[to]->k <= 1) {
        replaceRelaxedConstructIndex(initGraph, IndexFilePath, "LPA", true);
        return ;
    }

    shared_ptr<Community> c1 = nodeCommunityMap[from];
    shared_ptr<Community> c2 = nodeCommunityMap[to];
    
    while (c1->k > 2) {
        if (c1->parent && c1->parent->k < 2) break;
        c1 = c1->parent;
    }
    while (c2->k > 2) {
        if (c2->parent && c2->parent->k < 2) break;
        c2 = c2->parent;
    }
    if (c1 != c2) {
        deepMergeCommunities(c1, c2, nodeCommunityMap); //Merge C1 and C2 into C1.
        auto it = find(initCommunity->children.begin(), initCommunity->children.end(), c2);
        if (it != initCommunity->children.end()) {
            (*it)->parent = nullptr;
            initCommunity->children.erase(it);
        }
    }

    c1->graphptr = make_shared<Graph>(initGraph, c1->nodeIds);
    c1->graphptr->getAllEdgeSupports(); 
    GetTrussCommunitiesAndUpdateMap(nodeCommunityMap, 3, c1);

    assignCommunityIds(initCommunity);
    writeCommunityMapToFile(IndexFilePath);
}

void CSKNI::deepMergeCommunities(shared_ptr<Community> C1, shared_ptr<Community> C2, map<int, shared_ptr<Community>>& mymap) {

    for (auto nodeid : C2->nodeIds) {
        C1->nodeIds.insert(nodeid);
    }
    
    for (auto &child : C2->children) {
        if (find(C1->children.begin(), C1->children.end(), child) != C1->children.end()) continue;
        child->parent = C1;
        C1->children.push_back(child);
    }
    C1->graphptr = make_shared<Graph>(initGraph, C1->nodeIds);

    auto nodeids = C2->nodeIds;
    for (auto nodeid : nodeids) {
        if(mymap[nodeid] == C2) mymap[nodeid] = C1;
    }
}

shared_ptr<Community> CSKNI::findCommonAncestor(const vector<shared_ptr<Community>>& communities) {
    if (communities.empty()) return nullptr;
    
    // Use a set to store all ancestors of the first community.
    set<shared_ptr<Community>> ancestors;
    auto current = communities.front();
    if (communities.size() == 1) return current; 
    while (current) {
        ancestors.insert(current);
        current = current->parent;
    }

    // Traverse the ancestors of the remaining communities to find the closest common ancestor.
    shared_ptr<Community> commonAncestor = nullptr;
    for (size_t i = 1; i < communities.size(); ++i) {
        current = communities[i];
        while (current && ancestors.find(current) == ancestors.end()) {
            current = current->parent;
        }
        if (current) {
            if (commonAncestor == nullptr) {
                commonAncestor = current;
                continue;
            }
            if (commonAncestor->k < current->k) {
                continue;
            } else {
                commonAncestor = current;
                continue;
            }
        } else {
            return nullptr;
        }
    }

    return commonAncestor;
}

void CSKNI::assignCommunityIds(shared_ptr<Community> root) {
    if (!root) return; 
    
    int currentId = 1; 
    queue<shared_ptr<Community>> q; // Queue used for BFS.
    set<shared_ptr<Community>> visit; 

    root->id = currentId++;
    q.push(root);
    visit.insert(root);

    while (!q.empty()) {
        auto currentCommunity = q.front(); 
        q.pop();

        for (auto& childCommunity : currentCommunity->children) {
            if (childCommunity && visit.find(childCommunity) == visit.end()) {
                childCommunity->id = currentId++; 
                q.push(childCommunity); 
                visit.insert(childCommunity);
            }
        }
    }
}

void CSKNI::writeCommunityMapToFile(const string& filename) {
    ofstream outFile(filename);

    if (!outFile.is_open()) {
        cerr << "Unable to open file: " << filename << endl;
        return;
    }

    queue<shared_ptr<Community>> q;
    q.push(initCommunity); //Root node of the index

    while (!q.empty()) {
        shared_ptr<Community> current = q.front();
        q.pop();
        if (current) {
            outFile << "社区ID: " << current->id << ", 包含节点: ";
            for (int node : current->nodeIds) {
                outFile << node << " ";
            }
            outFile << ", k: " << current->k << ", 祖先ID: ";
            if (current->parent) {
                outFile << current->parent->id << ", ";
            } else {
                outFile << "-1, ";
            }
            outFile << "孩子节点ID: ";
            for (auto& child : current->children) {
                if (child) {
                    outFile << child->id << " ";
                }
            }
            outFile << endl;
        }
    
        for (const auto& child : current->children) {
            q.push(child);
        }
    }

    outFile.close();
}

void CSKNI::readCommunityMapFromFile(const string& filename) {
    
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Unable to open file: " << filename << endl;
        return;
    }

    string line;
    map<int, shared_ptr<Community>> tempCommunityMap; //Community ID mapped to community.
    map<int, vector<int>> childMap;

    while (getline(inFile, line)) {
        stringstream ss(line);
        string part, temp;
        int communityId, nodeId, k, parentId = -10;

        // Read Community ID
        getline(ss, part, ',');
        stringstream(part) >> temp >> communityId >> temp;

        // Creating a Community
        auto community = make_shared<Community>();
        community->id = communityId;

        // Read nodes
        getline(ss, part, ',');
        stringstream nodeStream(part.substr(part.find(":") + 1));
        while (nodeStream >> nodeId) {
            community->nodeIds.insert(nodeId);
        }

        // Read k value
        getline(ss, part, ',');
        stringstream(part) >> temp >> k;
        community->k = k;

        // Read Ancestor ID
        getline(ss, part, ',');
        stringstream(part) >> temp >> parentId;
        if (parentId != -1) {
            tempCommunityMap[parentId]->children.push_back(community);
            community->parent = tempCommunityMap[parentId];
        } else {
            initCommunity = community;
        }

        // Read child node ID
        getline(ss, part);
        stringstream childStream(part.substr(part.find(":") + 1));
        int childId;
        while (childStream >> childId) {
            if (childStream.peek() == ' ') childStream.ignore();
        }

        tempCommunityMap[communityId] = community;
    }

    // Rebuild nodeCommtyMap
    map<int, int> biaoJi; // The number of k layers corresponding to the node ID at this time
    for (const auto& pair : tempCommunityMap) {
        for (int nodeId : pair.second->nodeIds) {
            //Ensure that the map maps to the lowest level community
            if (biaoJi.find(nodeId) != biaoJi.end() && biaoJi[nodeId] >= pair.second->k) continue;
            nodeCommunityMap[nodeId] = pair.second;
            biaoJi[nodeId] = pair.second->k;
        }
    }

    inFile.close();
}

void CSKNI::runPythonScript(const string& AlgorithmName, const string& DataName) {
    string filePath = "Python/" + AlgorithmName + ".py";  

    Py_Initialize();

    if (!Py_IsInitialized()) {
        cerr << "Failed to initialize Python interpreter." << endl;
        return;
    }
    cout << "Python starts executing." << endl;

    wchar_t* wFilePath = Py_DecodeLocale(filePath.c_str(), NULL);
    wchar_t* wDataName = Py_DecodeLocale(DataName.c_str(), NULL);
    wchar_t* argv[] = {wFilePath, wDataName};
    PySys_SetArgvEx(2, argv, 0);


    FILE* pyFile = fopen(filePath.c_str(), "r");  
    if (pyFile != nullptr) {

        PyRun_SimpleFile(pyFile, filePath.c_str());
        fclose(pyFile); 
    } else {
        cerr << "Cannot reopen script file for Python: " << filePath << endl;
    }

    PyMem_Free(wFilePath);
    PyMem_Free(wDataName);

    Py_Finalize();
    cout << "Python execution ended" << endl;
}
 
set<shared_ptr<Community>> CSKNI::ReadFileAndReconstruct(const string& DataName) {
    string filePath = "data/" + DataName + "/" + DataName + "Basic.txt";
    ifstream file(filePath);  
    string line;
    set<shared_ptr<Community>> resultCommunity;

    if (!file.is_open()) {
        cerr << "Unable to open file: " << filePath << endl;
        return resultCommunity;
    }

    while (getline(file, line)) {
        istringstream iss(line);
        string temp;
        int node;
        set<int> community;

        iss >> temp >> temp;  
        while (iss >> node) {
            community.insert(node);

            if (iss.peek() == ',') iss.ignore();
        }
        shared_ptr<Community> curCommunity = make_shared<Community>(); //Generate a community
        initCommunity->children.push_back(curCommunity);
        curCommunity->parent = initCommunity;

        for (auto &nodeid : community) {
            nodeCommunityMap[nodeid] = curCommunity; 
            curCommunity->nodeIds.insert(nodeid); 
        }
     
        curCommunity->graphptr = make_shared<Graph>(initGraph, curCommunity->nodeIds);
        curCommunity->k = 2;
        resultCommunity.insert(curCommunity); 
    }
    file.close();  
    return resultCommunity;
}

void CSKNI::replaceRelaxedConstructIndex(const Graph& G, string IndexFilePath, const string AlgorithmName, bool flag) {
    shared_ptr<Community> G_0 = make_shared<Community>(G, 1); //Construct the original image into a community
    initCommunity = G_0; 
    nodeCommunityMap.clear();
    for (auto &nodeid : G.nodes) {
        nodeCommunityMap[nodeid.first] = G_0;
    }
    if (!flag) {
        readCommunityMapFromFile(IndexFilePath);
        return ;
    }

    size_t lastPos = IndexFilePath.rfind("//"); 
    size_t secondLastPos = IndexFilePath.rfind("//", lastPos - 1); 
    string dataName = IndexFilePath.substr(secondLastPos + 2, lastPos - secondLastPos - 2);

    //调用python算法
    runPythonScript(AlgorithmName, dataName);
    auto BCSet = ReadFileAndReconstruct(dataName); //Read the community file generated by the Python algorithm
    
    G_0->children.clear();
    for (auto &comBase : BCSet) {
        comBase->parent = G_0;
        G_0->children.push_back(comBase);
    }
    cout << "Number of basic communities: " << G_0->children.size() << endl;
    cout << "End of basic community" << endl;
    cout << "Start building truss:" << endl;
    constructIndexTreeAndUpdateMap(BCSet);
    cout << "Truss build completed." << endl;
    assignCommunityIds(G_0);
    writeCommunityMapToFile(IndexFilePath);
    return ;
}

void CSKNI::writeTOFileForCommunitySet(vector<shared_ptr<Community>> Com, string filename) {
    ofstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    for (auto &comPtr : Com) {
        file << "Community Info:" << endl;
        file << "Node Size: " << comPtr->nodeIds.size() << ", ";

        int edgesize = 0;
        vector<shared_ptr<Edge>> edgeSet; 
        for (auto &node : comPtr->nodeIds) {
            for (auto &edge : initGraph.nodes[node]->edges) {
                if (max(edge->from, edge->to) == node) continue;
                if (comPtr->nodeIds.find(max(edge->from, edge->to)) == comPtr->nodeIds.end()) continue;
                edgesize++;
                edgeSet.push_back(edge);
            }
        }

        file << "Edge Size: " << edgesize << endl;

        file << "Edges: " << endl;
        for (const auto& edge : edgeSet) {
            file << edge->from << " " << edge->to << endl;
        }
        file << endl;
    }
    file.close();
}

set<int> CSKNI::getRandomNodes(int n) {
    
    vector<int> nodeIds;
    for (auto nodePair : initGraph.nodes) nodeIds.push_back(nodePair.first);

    size_t numNodes = min(static_cast<size_t>(n), nodeIds.size());

    unsigned seed = chrono::high_resolution_clock::now().time_since_epoch().count();
    mt19937 g(seed);

    shuffle(nodeIds.begin(), nodeIds.end(), g);

    vector<int> selectedNodeIds(nodeIds.begin(), nodeIds.begin() + numNodes);
    set<int> success;
    for (auto x : selectedNodeIds) {
        success.insert(x);
    }
    return success;
}

vector<set<int>> CSKNI::GenerateQueryNodes(int searchNum, int n, const string &filename) {
    vector<set<int>> QuerySum;
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return QuerySum;
    }

    for (int i = 0; i < searchNum; i++) {
        set<int> querySingle = getRandomNodes(n);
        QuerySum.push_back(querySingle);
        for (auto &nodeid : querySingle) outFile << nodeid << " ";
        outFile << endl;
    }

    outFile.close();
    return QuerySum;
}