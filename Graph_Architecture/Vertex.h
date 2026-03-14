//
// Created by luca eaton on 3/11/26.
//

#ifndef VECTOR_H
#define VECTOR_H
#include <vector>
class Edge;
using namespace std;

class Vertex {
private:
    size_t id_;
    double lat_;
    double lon_;
    vector<Edge*> edges_;
public:
    Vertex();
    Vertex(long long i, double la, double lo);
    void addEdge(Edge *edge);
    [[nodiscard]] const std::vector<Edge *> &getEdges() const;
    [[nodiscard]] size_t getId() const;
    [[nodiscard]] double getLat() const;
    [[nodiscard]] double getLon() const;
};



#endif //VECTOR_H
