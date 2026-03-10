//
// Created by luca eaton on 3/9/26.
//

#include "Edge.h"
#include <utility>
#include <algorithm>
using namespace std;

/**
 * constructor
 *
 * @param to node point to
 * @param len length of road
 * @param limit speed limit of road
 * @param name name of the street
 */
Edge::Edge(const int to, const double len, double limit, string name)
    : toNode(to), streetName(move(name)), distance(len), speedLimit(limit) {}

/**
 *
 * Time is equal to Distance/Speed.
 * Time determines how long a road actual is, its weight.
 * Using this logic we can assume that weight will equal to W = (D/S)
 * Where D = Distance, S = Speed.
 *
 * @return weight of the edge
 */
double Edge::calcWeight() const {
    const double s = std::max(speedLimit, 1.0);
    return distance / s;
}
