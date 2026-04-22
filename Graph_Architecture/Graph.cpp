#include "Graph.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <unordered_set>

#include "Edge.h"
#include "Vertex.h"

Graph::Graph(const size_t vertexCount, const size_t edgeCount) {
    vertices_.reserve(vertexCount);
    edges_.reserve(edgeCount);
    streetMap_.reserve(edgeCount);
}

/**
 * @note
 * Adds a vertex to the map for later look up for vertex ids
 *
 * @param id the id of the vertex
 * @param lat latitude of vertex
 * @param lng longitude of vertex
 *
 * @timecomplex O(1)
 * @spacecomplex O(V),V stands for # of nodes
 */
void Graph::addVertx(long long id, double lat, double lng) {
    vertices_.try_emplace(id, std::make_unique<Vertex>(id, lat, lng));
}

/**
 * @note
 * Adds a street name to the map for later look up for edge ids
 *
 * @relates Graph::streetMap_
 * @param id id of edge
 * @param street street name
 *
 * @timecomplex O(1)
 * @spacecomplex O(E), E stands for # of edges
 */
void Graph::addStreet(long long id, std::string street) {
    streetMap_[std::move(street)].push_back(id);
}

/**
 * @note
 * Looks within the vertices map to find the vertex associated with the id
 *
 * @relates Graph::vertices_
 * @param id id of the wanted Vertex
 * @return vertex object from the unordered_map
 *
 * @timecomplex O(1)
 */
Vertex *Graph::getVertex(long long id) const {
    const auto it = vertices_.find(id);
    return (it != vertices_.end()) ? it->second.get() : nullptr;
}

/**
 * @note
 * Adds an Edge to the Edge list towards the source and destination node
 * Gathers the ids and retrieves the nodes and check if they exist
 * Create the unique ptr object of the edge and add it to the individual edge list to each node
 * Shift the ownership to the Graph Object.
 *
 * @relates Graph::edges_
 * @param id edge id
 * @param src source vertex
 * @param dst destination vertex
 * @param dist distance of the street
 * @param sL max speed of the street
 * @param sN street name
 * @param sF
 *
 * @timecomplex O(1)
 */
void Graph::addEdge(long long id, Vertex *src, Vertex *dst, double dist, double sL, std::string sN, double sF) {
    if (!src || !dst) return; //don't add if they don't exist

    //unique pointer
    auto e = std::make_unique<Edge>(id, src, dst, dist, sL, std::move(sN), sF);
    Edge *rAddress = e.get();
    src->addEdge(rAddress), dst->addEdge(rAddress); // register the same edge
    edges_[id] = (std::move(e)); // transfer ownership so the edge object lives on.
}

/**
 * @note
 * Collects all vertex IDs associated with a given street name.
 * Since a street is composed of multiple edge segments, each segment
 * contributes both its source and destination vertex to the result set.
 * This allows Dijkstra to treat the entire street as a valid start or
 * end point rather than a single arbitrary segment.
 *
 * @relates Graph::streetMap_
 * @relates Graph::edges_
 * @param name the street name to look up
 * @return an unordered_set of vertex IDs belonging to that street,
 *         or an empty set if the street is not found
 *
 * @timecomplex O(S), S = number of segments on the street
 * @spacecomplex O(S), storing up to 2*S vertex IDs
 */
unordered_set<long long> Graph::nameToVertices(const string &name) const {
    unordered_set<long long> result;
    const auto it = streetMap_.find(name);
    if (it == streetMap_.end()) return result;
    for (long long id: it->second) {
        const auto eit = edges_.find(id);
        if (eit == edges_.end()) continue; // if this edge id doesn't exist (it should) skip
        result.insert(eit->second->getSrc()->getId());
        result.insert(eit->second->getDst()->getId());
    }
    return result;
}

/**
 * @note
 * Finds the vertex within a set that is furthest away from a reference set of vertices.
 * For each vertex in 'from', we calculate its minimum distance to any vertex in 'reference'
 * (how close it is to the reference street). We then return whichever vertex in 'from'
 * has the largest such minimum distance, meaning it is the furthest away from the reference street.
 *
 * This is used to find a realistic start/end point for routing between two streets.
 * For example, given "11th Avenue" and "West 52nd Street", this function returns
 * the vertex on 11th Avenue that is furthest from West 52nd Street, so that
 * Dijkstra traverses the most realistic path between the two streets.
 *
 * @param from      set of vertex IDs to search through (all vertices on street A)
 * @param reference set of vertex IDs to measure distance against (all vertices on street B)
 * @return the vertex ID in 'from' that is furthest from 'reference', or -1 if 'from' is empty
 *
 * @timecomplex O(F * R), F = size of from, R = size of reference
 * @spacecomplex O(1)
 */
long long Graph::findNode(const unordered_set<long long> &from, const unordered_set<long long> &reference) const {
    long long best = -1;
    double bestDist = -1;
    for (const long long id: from) {
        const Vertex *v = getVertex(id);
        // find min distance to any vertex in reference set
        double minToRef = numeric_limits<double>::infinity();
        for (long long refId: reference) {
            const Vertex *r = getVertex(refId);
            double d = pow(v->getLat() - r->getLat(), 2) + pow(v->getLon() - r->getLon(), 2);
            minToRef = min(minToRef, d);
        }
        // pick the source vertex that is furthest from the reference street
        if (minToRef > bestDist) {
            bestDist = minToRef;
            best = id;
        }
    }
    return best;
}

/**
 *
 * @param id id of edge wanted
 * @return the edge object
 */
Edge *Graph::getEdge(const long long id) const {
    const auto it = edges_.find(id);
    return (it != edges_.end()) ? it->second.get() : nullptr;
}

/**
 * @note
 * Look up map for all existing vertices
 *
 * @relates Graph::vertices_
 * @return an unordered_map of ids to vertex
 *
 * @spacecomplex O(V) # of vertices
 */
const std::unordered_map<long long, std::unique_ptr<Vertex> > &Graph::getVertices() const {
    return vertices_;
}

ostream operator<<(const ostream &lhs, const chrono::duration<long long, ratio<1, 1000> > &rhs);


Graph::RouteResult Graph::Dijkstra(const string &nameA, const string &nameB) const {
    const auto t_start = std::chrono::high_resolution_clock::now();
    unordered_set<long long> vst; // visited list
    unordered_map<long long, double> dist; // distance list (cost)
    unordered_map<long long, long long> prev;
    // set all indexes to INF
    for (const auto &id: getVertices() | views::keys) dist[id] = std::numeric_limits<double>::infinity();

    const auto sourcesA = nameToVertices(nameA);
    const auto targetsB = nameToVertices(nameB);
    if (sourcesA.empty() || targetsB.empty()) return {};

    const long long src = findNode(sourcesA, targetsB);
    const long long target = findNode(targetsB, sourcesA);
    if (src < 0 || target < 0) return {};

    priority_queue<pair<double, long long>, vector<pair<double, long long> >, std::greater<> > pq;
    dist[src] = 0.0;
    pq.emplace(0.0, src);

    while (!pq.empty()) {
        const long long currId = pq.top().second;
        pq.pop();
        if (vst.contains(currId)) continue;
        const Vertex *current = getVertex(currId);
        vst.insert(currId);
        for (auto u: current->getEdges()) {
            auto b = u->getNeighbor(current); // target node of curr edge
            auto w = u->getWeight(); // weight of our edge
            if (dist[currId] + w < dist[b->getId()]) {
                dist[b->getId()] = dist[currId] + w;
                prev[b->getId()] = currId; // record path
                pq.emplace(dist[b->getId()], b->getId());
            }
        }
    }

    const auto t_end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);

    const double best = dist[target];
    if (best == std::numeric_limits<double>::infinity()) {
        std::cout << "No path possibly between " << nameA << " and " << nameB << "." << std::endl;
        return {};
    }
    std::cout << "\n(Dijkstra) Shortest travel time between " << nameA << " and " << nameB << " : "
            << std::ceil(best) << " mins\n";
    std::cout << "Time Taken :" << duration << std::endl;

    // reconstruct path by walking back through prev
    RouteResult result;
    result.travelTime = std::ceil(best);
    // start at the target and walk backwards through prev
    // we stop once we hit the source node
    for (long long cur = target; cur != src; cur = prev[cur]) {
        const Vertex *v = getVertex(cur);
        result.path.emplace_back(v->getLat(), v->getLon());
    }
    const Vertex *srcV = getVertex(src);
    result.path.emplace_back(srcV->getLat(), srcV->getLon());
    // reverse since its target -> src we want src -> target
    ranges::reverse(result.path);

    return result;
}

/**
 * @note
 * The size of the graph aka the amount of nodes within it
 *
 * @return # of nodes
 */
size_t Graph::size() const {
    return vertices_.size();
}

/**
 * @note
 * A print function in order to better visualize what vertex points to what
 */
void Graph::print() const {
    for (const auto &[id, vertex]: vertices_) {
        std::cout << "Vertex " << id << " (" << vertex->getLat() << ", " << vertex->getLon() << ")\n";
        for (const Edge *e: vertex->getEdges()) {
            const Vertex *neighbor = e->getNeighbor(vertex.get());
            std::cout << "  → " << neighbor->getId()
                    << " | " << e->getStreetName()
                    << " | dist: " << e->getDistance()
                    << " | weight: " << e->getWeight()
                    << " | free flow speed: " << e->getFreeFlowS()
                    << " | speed : " << e->getSpeedLimit() << std::endl;
        }
    }
}

/**
 *
 * @return the id -> edge look up map
 */
const std::unordered_map<long long, std::unique_ptr<Edge> > &Graph::getEdges() const {
    return edges_;
}
