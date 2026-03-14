#include "Vertex.h"
/*
 * constructor
 */
Vertex::Vertex() : id_(0ll), lat_(0.0), lon_(0.0) {}
Vertex::Vertex(const long long i, const double la, const double lo) : id_(i), lat_(la), lon_(lo) {}
/**
 * @note
 * Pushes an edge object within the edges vector
 *
 * @relates Vertex::edges_
 * @param edge adds edges to the edge list
 */
void Vertex::addEdge(Edge *edge) {
    edges_.push_back(edge);
};
/**
 * @note
 * allows for us to access the edge list for traversal
 *
 * @relates Vertex::edges_
 * @return pointer to edge list
 */
const std::vector<Edge*>& Vertex::getEdges() const {
    return edges_;
}
/**
 * @notes
 * allows for easy look up for the id of a vertex
 *
 * @return id number of node
 */
long long Vertex::getId() const { return id_;}
/**
 * @note
 * allows for easy look up for the latitude of a vertex
 *
 * @return latitude of node
 */
double Vertex::getLat() const { return lat_;}
/**
 * @note
 * allows for easy look up for the longitude of a vertex
 *
 * @return longitude of node
 */
double Vertex::getLon() const { return lon_;}
