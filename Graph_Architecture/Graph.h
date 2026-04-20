#ifndef GRAPH_H
#define GRAPH_H
#include <unordered_map>
#include <vector>
#include <memory>
#include "Vertex.h"
#include "Edge.h"

class Graph {
    public:
        Graph(size_t vertexCount, size_t edgeCount);
        void addVertx(long long id, double lat, double lng);
        void addStreet(long long id, std::string street);
        [[nodiscard]] Vertex *getVertex(long long id) const;
        void addEdge(long long id, Vertex *src, Vertex *dst, double dist, double sL, std::string sN, double sF);
        [[nodiscard]] Edge *nameToEdge(const string &name) const;
        [[nodiscard]] Edge *getEdge(long long id) const;
        [[nodiscard]] const std::unordered_map<long long, std::unique_ptr<Vertex>> &getVertices() const;
        [[nodiscard]] double Dijkstra(const Edge &streetA, const Edge &streetB) const;
        [[nodiscard]] size_t size() const;
        void print() const;
        [[nodiscard]] const unordered_map<long long, std::unique_ptr<Edge>>& getEdges() const;

    private:
        unordered_map<long long, std::unique_ptr<Vertex>> vertices_;
        unordered_map<string, vector<long long>> streetMap_;
        unordered_map<long long, std::unique_ptr<Edge>> edges_;
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
