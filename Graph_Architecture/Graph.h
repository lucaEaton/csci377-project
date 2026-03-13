//
// Created by luca eaton on 3/11/26.
//

#ifndef GRAPH_H
#define GRAPH_H
#include <unordered_map>
#include <vector>
#include <memory>
#include "Vertex.h"
#include "Edge.h"
using namespace std;

class Graph {
private:
    unordered_map<long long, std::unique_ptr<Vertex>> vertices_;
    vector<std::unique_ptr<Edge>> edges_;

public:
    Graph(size_t vertexCount, size_t edgeCount);
    void addVertx(long long id, double lat, double lng);

    [[nodiscard]] Vertex *getVertex(long long id) const;

    void addEdge(long long id, Vertex* srcId, Vertex* dstId, double dist, double sL, std::string sN);

    const std::unordered_map<long long, std::unique_ptr<Vertex>> &getVertices() const;

    void print() const;
};

//NOTES
// We need an edge list to begin with. Idk how we get that edge list but its possible
/*
 * for (const auto& e : edge)
 *      adj[e[0]].push_back(e[1]);
 * Assume I have edges_ = vector<Edge*>
 * for (const auto& e : edges_)
 *  Vertex* src = edge->getSource();
 *  Vertex* dest = edge->getDest();
 *  adj[src].push_back(dest);
*/
// {v1 : v2, ..., vn}
// unordered_map<Vertex, vector<Vertex>>
// you have to define a custom hash function for vertices

#endif //GRAPH_H
