#include "Edge.h"
#include <utility>
#include <algorithm>
using namespace std;
/**
 * constructor
 */
// NOTES - you can define two constructors and overload them. One default and one parameter
Edge::Edge() : id_(0ll), streetName_(""), distance_(0.0), speedLimit_(0.0), weight_(0.0), srcNode_(nullptr), dstNode_(nullptr) {}
Edge::Edge(const long long id, Vertex* src, Vertex* dst, const double len, const double limit, string name)
    : id_(id), streetName_(move(name)), distance_(len), speedLimit_(limit), srcNode_(src), dstNode_(dst) {
    weight_ = calcWeight();
}
/**
 * @note
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



