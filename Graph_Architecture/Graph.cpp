//
// Created by luca eaton on 3/11/26.

#include "Graph.h"

#include <iostream>

#include "Edge.h"
#include "Vertex.h"

Graph::Graph(const size_t vertexCount, const size_t edgeCount) {
    vertices_.reserve(vertexCount);
    edges_.reserve(edgeCount);
}

/**
 *
 * All data will be found when parsed through the data
 *
 * @param id the id of the vertex
 * @param lat latitude of vertex
 * @param lng longitude of vertex
 */
void Graph::addVertx(long long id, double lat, double lng) {
    vertices_.try_emplace(id, std::make_unique<Vertex>(id, lat, lng));
}
/**
 *
 * @param id id of the wanted Vertex
 * @return vertex object from the unordered_map
 */
Vertex* Graph::getVertex(long long id) const {
    const auto it = vertices_.find(id);
    return (it != vertices_.end()) ? it->second.get() : nullptr;
}
/**
 *
 * Adds an Edge to the Edge list towards the source and destination node
 * Gathers the ids and retrieves the nodes and check if they exist
 * Create the unique ptr object of the edge and add it to the individual edge list to each node
 * Shift the ownership to the Graph Object.
 *
 * @param srcId source id node
 * @param dstId destination id node
 * @param dist distance of the street
 * @param sL max speed of the street
 * @param sN street name
 */
void Graph::addEdge(long long id, Vertex* src, Vertex* dst, double dist, double sL, std::string sN) {
    if (!src || !dst) return; //don't add if they don't exist

    //unique pointer
    auto e = std::make_unique<Edge>(id, src, dst, dist, sL, std::move(sN));
    Edge* rAddress = e.get();
    src->addEdge(rAddress), dst->addEdge(rAddress); // register the same edge
    edges_.push_back(std::move(e)); // transfer ownership so the edge object lives on.
}
/**
 *
 * @return a loop up map for all vertices
 */
const std::unordered_map<long long, std::unique_ptr<Vertex>>& Graph::getVertices() const {
    return vertices_;
}

/*
 * Demo - from textbook
 *
    for (int i = 1; i <= n; i++) {
        distance[i] = INF;
    }
    distance[x] = 0;
    q.push({0,x});
    while (!q.empty()) {
        int a = q.top().second; q.pop();
        if (processed[a]) continue;
        processed[a] = true;
        for (auto u : adj[a]) {
            int b = u.first, w = u.second;
            if (distance[a]+w < distance[b]) {
                distance[b] = distance[a]+w;
                q.push({-distance[b],b});
            }
        }
    }
 *
 */
// int Graph::Dijkstra(Graph g){}




/**
 * print function for debugging
 */
void Graph::print() const {
    for (const auto& [id, vertex] : vertices_) {
        std::cout << "Vertex " << id << " (" << vertex->getLat() << ", " << vertex->getLon() << ")\n";
        for (const Edge* e : vertex->getEdges()) {
            const Vertex* neighbor = e->getNeighbor(vertex.get());
            std::cout << "  → " << neighbor->getId()
                      << " | " << e->getStreetName()
                      << " | dist: " << e->getDistance()
                      << " | weight: " << e->getWeight() << "\n";
        }
    }
}


