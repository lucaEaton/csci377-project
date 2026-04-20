#ifndef VECTOR_H
#define VECTOR_H
#include <vector>
class Edge;
using namespace std;

class Vertex {
    public:
        Vertex();
        Vertex(long long i, double la, double lo);
        void addEdge(Edge *edge);
        [[nodiscard]] const std::vector<Edge *> &getEdges() const;
        [[nodiscard]] long long getId() const;
        [[nodiscard]] double getLat() const;
        [[nodiscard]] double getLon() const;

    private:
        long long id_;
        double lat_;
        double lon_;
        vector<Edge*> edges_;
};



#endif //VECTOR_H
