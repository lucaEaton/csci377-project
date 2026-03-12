//
// Created by luca eaton on 3/11/26.
//

#include "Vertex.h"
/*
 * constructor
 */
Vertex::Vertex() : id_(0ll), lat_(0.0), lon_(0.0) {}
Vertex::Vertex(const long long i, const double la, const double lo) : id_(i), lat_(la), lon_(lo) {}
/**
 *
 * @param edge adds edges to the edge list
 */
void Vertex::addEdge(Edge *edge) {
    edges_.push_back(edge);
};
/**
 *
 * @return pointer to edge list
 */
const std::vector<Edge*>& Vertex::getEdges() const {
    return edges_;
}
/**
 *
 * @return id number of node
 */
size_t Vertex::getId() const { return id_;}
/**
 *
 * @return latitude of node
 */
double Vertex::getLat() const { return lat_;}
/**
 *
 * @return longitude of node
 */
double Vertex::getLon() const { return lon_;}
