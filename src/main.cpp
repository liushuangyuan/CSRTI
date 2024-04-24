#include "../include/CSKNI.h"
vector<set<int>> readQueryNodeFromFile(const string& filename);

int main(int argc, char* argv[]) {

    if (argc != 8) {
        cerr << "Usage: " << argv[0] << " <string: Dataset> <string: Community Discovery Algorithm Name> <int: k> <int: searchNum> <int: singleQueryNum> <bool: Whether to generate query> <bool: Whether to rebuild the index>" << endl;
        cerr << "Example: " << argv[0] << " Testdataset LPA 3 10 1 true true" << endl;
        return 1;
    }
    
    string DatasetName = argv[1];
    string communityDiscoveryName = argv[2];
    int k = stoi(argv[3]);
    int searchNum = stoi(argv[4]); //Number of searches
    int n = stoi(argv[5]); //Number of nodes in a single search
    bool generateQueryData = (string(argv[6]) == "true");
    bool rebuildIndex = (string(argv[7]) == "true");

    const string initPath = "data//";
    const string finePath = initPath + DatasetName + "//" + DatasetName;

    string FilePath = finePath + ".txt";
    string IndexFilePath = finePath + "Index.txt";
    string ResultFilePath = finePath + "Result.txt";
    string QueryFilePath = finePath + "Query.txt";
    string DynamicFilePath = finePath + "Dynamic.txt";

    CSKNI cskni; 
    //Read graph data file
    cskni.getInitGraph().loadGraph(FilePath); 


    if (rebuildIndex && filesystem::exists(DynamicFilePath)) filesystem::remove(DynamicFilePath);
    // Building an index
    cskni.replaceRelaxedConstructIndex(cskni.getInitGraph(), IndexFilePath, communityDiscoveryName, rebuildIndex);
    
    //Read or generate query nodes
    vector<set<int>> queryNodeVector;
    if (generateQueryData) queryNodeVector = cskni.GenerateQueryNodes(searchNum, n, QueryFilePath);
    else queryNodeVector = readQueryNodeFromFile(QueryFilePath);

    cout << "Start querying: " << endl;
    int i = 0;
    for (auto queryNodes : queryNodeVector) {
        if (i >= searchNum) break;
        //Start querying
        set<int> Result = cskni.NCS(queryNodes, k); 
        cskni.writeSearchResultsToFile(ResultFilePath, Result, queryNodes, k); 
        i++;
    }
    cout << "Query completed" << endl;
    return 0;
}

vector<set<int>> readQueryNodeFromFile(const string& filename) {
    
    vector<set<int>> QueryNum;
    ifstream file(filename);
    string line;

    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return QueryNum; 
    }
    
    while (getline(file, line)) {
        istringstream iss(line);
        int num;
        set<int> querySingle;
        while (iss >> num) {
            querySingle.insert(num);
        }
        if (querySingle.empty()) continue;
        QueryNum.push_back(querySingle);
    }
    
    file.close();  
    return QueryNum; 
}