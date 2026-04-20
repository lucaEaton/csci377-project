#ifndef EDGE_H
#define EDGE_H
#include <string>
#include <nlohmann/json.hpp>

using namespace std;

class Vertex;
class Edge {
    public:
        [[nodiscard]] double calcWeight() const;
        Vertex *getNeighbor(const Vertex *from) const;
        [[nodiscard]] Vertex *getSrc() const;
        [[nodiscard]] Vertex *getDst() const;
        [[nodiscard]] string getStreetName() const;
        [[nodiscard]] double getDistance() const;
        [[nodiscard]] double getSpeedLimit() const;
        [[nodiscard]] double getWeight() const;

        double getFreeFlowS() const;

        void setWeight(double w);

        void setSpeed(double s);

        void setFreeFlowS(double f);

        double setLiveWeight(double s, double f);

        Edge();

        Edge(long long id, Vertex *src, Vertex *dst, double len, double limit, string name, double ffSpeed);


    private:
        long long id_;
        string streetName_;
        double distance_;
        double speed_;
        double ffSpeed_;
        double weight_;
        Vertex *srcNode_;
        Vertex* dstNode_;
};

#endif //EDGE_H
