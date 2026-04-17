#include "Edge.h"
#include <utility>
#include <algorithm>
#include <iostream>
using namespace std;
/**
 * constructor
 */
// NOTES - you can define two constructors and overload them. One default and one parameter
Edge::Edge() : id_(0ll), streetName_(""), distance_(0.0), speedLimit_(0.0), weight_(0.0), srcNode_(nullptr), dstNode_(nullptr) {}

Edge::Edge(const long long id, Vertex* src, Vertex* dst, const double len, const double limit, string name, const double ffSpeed)
    : id_(id), streetName_(move(name)), distance_(len), speedLimit_(limit), srcNode_(src), dstNode_(dst), ffSpeed_(ffSpeed) {
    weight_ = calcWeight();
}
/**
 * @note
 * Travel time is equal to Distance / Speed, converted to minutes.
 * This determines the cost (weight) of traversing an edge.
 *
 * W = (D / S) * 60
 * Where D = Distance in km, S = Current speed in km/h (sourced from TomTom live traffic).
 *
 * Using live current speed rather than the static speed limit allows the weight
 * to reflect real-time traffic conditions
 *
 * a road with heavy congestion will have a lower current speed,
 * increasing its weight and making Dijkstra's
 * algorithm prefer less congested routes.
 *
 * @return weight of the edge in minutes
 */
double Edge::calcWeight() const {
    const double s = max(speedLimit_, 1.0); // live speed
    const double ff = max(ffSpeed_, 1.0); // free flow speed
    //std::cout << "dist: " << distance_ << " speed: " << speedLimit_ << " weight: " << (distance_/speedLimit_)*60.0 << "\n";
    return ((distance_ / ff) * 60.0) * ( ff / s); // base time * how contested the road is
}
/**
 * @note
 * to see where a source vertex is pointing to within an edge, vise versa
 *
 * @param from a vertex
 * @return its neighboring node
 */
Vertex* Edge::getNeighbor(const Vertex* from) const {return from == srcNode_ ? dstNode_ : srcNode_;}
/**
 * @note
 * allows for easy lookup for an edges source vertex
 *
 * @return source node of an edge
 */
Vertex* Edge::getSrc() const { return srcNode_; }
/**
 * @note
 * allows for easy lookup for an edges destination vertex
 *
 * @return the destination node of an edge
 */
Vertex* Edge::getDst() const { return dstNode_; }
/**
 * @note
 * allows for easy look up for an edges street name
 *
 * @return the edge street name
 */
string Edge::getStreetName() const {
    return streetName_;
}
/**
 * @note
 * allows for easy lookup for an edges distance
 *
 * @return distance of the edge
 */
double Edge::getDistance() const {
    return distance_;
}
/**
 * @note
 * allows for easy lookup for an edges speed limit
 *
 * @return speed limit
 */
double Edge::getSpeedLimit() const {
    return speedLimit_;
}
/**
 * @note
 * allows for easy lookup for an edges weight
 *
 * @return edge weight
 */
double Edge::getWeight() const {
    return weight_;
}



