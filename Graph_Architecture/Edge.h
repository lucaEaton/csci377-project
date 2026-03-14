#ifndef EDGE_H
#define EDGE_H
#include <string>
#include <nlohmann/json.hpp>

using namespace std;
class Vertex;
class Edge {
private:
    long long id_;
    string streetName_;
    double distance_;
    double speedLimit_;
    double weight_;
    Vertex *srcNode_;
    Vertex* dstNode_;
public:
    [[nodiscard]] double calcWeight() const;
    Vertex *getNeighbor(const Vertex *from) const;
    [[nodiscard]] Vertex *getSrc() const;
    [[nodiscard]] Vertex *getDst() const;
    [[nodiscard]] string getStreetName() const;
    [[nodiscard]] double getDistance() const;
    [[nodiscard]] double getSpeedLimit() const;
    [[nodiscard]] double getWeight() const;
    Edge();
    Edge(long long id, Vertex* src, Vertex* dst, const double len, double limit, string name);
};

#endif //EDGE_H
