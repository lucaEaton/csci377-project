//
// Created by luca eaton on 3/9/26.
//

#include "Edge.h"
#include <utility>
#include <algorithm>
using namespace std;
/**
 * constructor
 */
// NOTES - you can define two constructors and overload them. One default and one parameter
Edge::Edge() : streetName_(""), distance_(0.0), speedLimit_(0.0), weight_(0.0), srcNode_(nullptr), dstNode_(nullptr) {}
Edge::Edge(Vertex* src, Vertex* dst, double len, double limit, string name)
    : streetName_(move(name)), distance_(len), speedLimit_(limit), srcNode_(src), dstNode_(dst) {
    weight_ = calcWeight();
}
// add getters and setters for your edge class
/**
 *
 * Time is equal to Distance/Speed.
 * Time determines how long a road actual is, its weight.
 *
 * Using this logic we can assume that weight will equal to W = (D/S)
 * Where D = Distance, S = Speed.
 *
 * @return weight of the edge
 */

double Edge::calcWeight() const {
    const double s = max(speedLimit_, 1.0);
    return distance_ / s;
}
/**
 *
 * @param from a vertex
 * @return its neighboring node
 */
Vertex* Edge::getNeighbor(const Vertex* from) const {return from == srcNode_ ? dstNode_ : srcNode_;}
/**
 *
 * @return source node of an edge
 */
Vertex* Edge::getSrc() const { return srcNode_; }
/**
 *
 * @return the destination node of an edge
 */
Vertex* Edge::getDst() const { return dstNode_; }
/**
 *
 * @return the edge street name
 */
string Edge::getStreetName() const {
    return streetName_;
}
/**
 *
 * @return distance of the edge
 */
double Edge::getDistance() const {
    return distance_;
}
/**
 *
 * @return speed limit
 */
double Edge::getSpeedLimit() const {
    return speedLimit_;
}



