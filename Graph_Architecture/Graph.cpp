#include "Graph.h"
#include <iostream>
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
Vertex* Graph::getVertex(long long id) const {
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
 *
 * @timecomplex O(1)
 */
void Graph::addEdge(long long id, Vertex* src, Vertex* dst, double dist, double sL, std::string sN) {
    if (!src || !dst) return; //don't add if they don't exist

    //unique pointer
    auto e = std::make_unique<Edge>(id, src, dst, dist, sL, std::move(sN));
    Edge* rAddress = e.get();
    src->addEdge(rAddress), dst->addEdge(rAddress); // register the same edge
    edges_[id] = (std::move(e)); // transfer ownership so the edge object lives on.
}
/**
 * @note
 * Within the Graph class we store two things
 * an unordered map of names to street to their vector of ids (since there are multiple edges that are on the same street)
 * and an unordered map of ids to the edge object itself
 *
 * my theory is if we have a list of all the possible street names then we can get its edge object simply from its name
 * by looking up the name through our street map then the id of that output in our edge map
 *
 * Through this we can successfully find the Edge object through the name of the street
 *
 * @relates Graph::streetMap_
 * @relates Graph::edges_
 * @param name name of streets
 * @return the edge object of that street
 *
 * @timecomplex O(1)
 */
Edge* Graph::nameToEdge(const string &name) const {
    const long long id = streetMap_.find(name)->second[0];
    return edges_.find(id)->second.get();
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
const std::unordered_map<long long, std::unique_ptr<Vertex>>& Graph::getVertices() const {
    return vertices_;
}
// as of rn returns how long it would take in mins
double Graph::Dijkstra(const Edge &streetA, const Edge &streetB) const {
    unordered_map<long long, bool> vst; // visited list
    unordered_map<long long, double> dist; // distance list (cost)
    // set all indexes to INF
    for (const auto &id: getVertices() | views::keys) dist[id] = INT_MAX;
    const Vertex* src = streetA.getSrc();
    const Vertex* target = streetB.getDst();
    priority_queue<pair<int, long long>, vector<pair<int, long long>>, std::greater<>> pq;
    dist[src->getId()] = 0;
    pq.push({0, src->getId()});
    while (!pq.empty()) {
        long long currId = pq.top().second;
        Vertex* current = getVertex(currId);
        pq.pop();
        if (vst.contains(current->getId())) continue;
        vst[current->getId()] = true;
        for (auto u : current->getEdges()) {
            auto b = u->getNeighbor(current); // target node of curr edge
            auto w = u->getWeight(); // weight of our egde
            if (dist[current->getId()] + w < dist[b->getId()]) {
                dist[b->getId()] = dist[current->getId()] + w;
                pq.push({dist[b->getId()], b->getId()});
            }
        }
    }
    if (dist[target->getId()] * 60 >= INT_MAX) {
        std::cout << "No path possibly between" << streetA.getStreetName() << " and " << streetB.getStreetName() << "." << std::endl;
    } else {
        std::cout << "Travel time between "<< streetA.getStreetName() << " and " << streetB.getStreetName() << " : "<< dist[target->getId()] * 60 << " mins";
    }

    return dist[target->getId()] * 60;
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


