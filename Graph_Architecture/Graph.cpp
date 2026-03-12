//
// Created by luca eaton on 3/11/26.

#include "Graph.h"
#include "Edge.h"
#include "Vertex.h"
/**
 *
 * All data will be found when parsed through the data
 *
 * @param id the id of the vertex
 * @param lat latitude of vertex
 * @param lng longitude of vertex
 */
void Graph::addVertx(long long id, double lat, double lng) {
    if (const long long node_id = id; !vertices_.contains(node_id)) {
        vertices_[node_id] = std::make_unique<Vertex>(id, lat, lng);
    }
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
void Graph::addEdge(long long srcId, long long dstId, double dist, double sL, std::string& sN) {
    Vertex* src = getVertex(srcId);
    Vertex* dst = getVertex(dstId);
    if (!src || !dst) return; //don't add if they don't exist

    //unique pointer
    const auto e = std::make_unique<Edge>(src, dst, dist, sL, sN);
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