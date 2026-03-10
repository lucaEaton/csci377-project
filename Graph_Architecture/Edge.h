//
// Created by luca eaton on 3/9/26.
//

#ifndef EDGE_H
#define EDGE_H
#include <string>
using namespace std;


class Edge {
public:
    int toNode;
    string streetName;
    double distance;
    double speedLimit;

    double calcWeight() const;

    Edge(int to, double len, double limit, std::string name);
};



#endif //EDGE_H
