#ifndef CSKNI_H_
#define CSKNI_H_
#include "Community.h"

class CSKNI { 
public:   
    // Construct index tree algorithm
    void constructIndexTreeAndUpdateMap(set<shared_ptr<Community>>& BCSet);

    //Get truss community algorithm
    void GetTrussCommunitiesAndUpdateMap(map<int, shared_ptr<Community>>& nodeToCommunityMap, int k, const shared_ptr<Community> &C);

    // Search entry
    set<int> NCS(const set<int>& Q, int k);

    Graph &getInitGraph() {
        return initGraph;
    }

    // Save the search results in the specified file
    void writeSearchResultsToFile(const string& filename, const set<int>& communityNodes, const set<int>& queryNodes, int k);

    // Dynamically maintain the index
    void IndexAddEdge(int from, int to, string IndexFilePath, string DynamicFilePath);
    void IndexAddNode(int addNodeId, set<int> neiNodeId, string IndexFilePath, string DynamicFilePath);
    void IndexRemoveNode(int RemoveNode, string IndexFilePath, string DynamicFilePath);
    void IndexRemoveEdge(int from, int to, string IndexFilePath, string DynamicFilePath);

    //Interface for invoking Python scripts
    void runPythonScript(const string& AlgorithmName, const string& DataName);
    set<shared_ptr<Community>> ReadFileAndReconstruct(const string& DataName);

    //Index construction
    void replaceRelaxedConstructIndex(const Graph& G, string IndexFilePath, const string AlgorithmName, bool flag = true);

    //Write the community information into the file
    void writeTOFileForCommunitySet(vector<shared_ptr<Community>> Com, string filename);

    //Generate and write query nodes to the file
    vector<set<int>> GenerateQueryNodes(int searchNum, int n, const string &filename);
private:
    map<int, shared_ptr<Community>> nodeCommunityMap; // Mapping of node IDs to communities
    Graph initGraph;
    shared_ptr<Community> initCommunity = nullptr; 

    //Merge C2 into C1
    void deepMergeCommunities(shared_ptr<Community> C1, shared_ptr<Community> C2, map<int, shared_ptr<Community>>& mymap);

    //Find the common ancestor of the communities
    shared_ptr<Community> findCommonAncestor(const vector<shared_ptr<Community>>& communities);

    //Assign numbers to the communities of the entire tree
    void assignCommunityIds(shared_ptr<Community> root);

    //Write the index to a file
    void writeCommunityMapToFile(const string& filename);
    //Read the index file and rebuild the index
    void readCommunityMapFromFile(const std::string& filename);
    //Randomly obtain n nodes
    set<int> getRandomNodes(int n);
};

#endif