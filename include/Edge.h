#ifndef EDGE_H_
#define EDGE_H_

class Edge {
public:
    int from; // min
    int to; // max
    int weight; 
    int support = 0;

    Edge(int from, int to, int weight = 1, int support = 0) : from(from), to(to), weight(weight), support(support) {} 
};

#endif