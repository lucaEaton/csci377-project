#include <iostream>
#include "Graph_Architecture/Graph.h"
#include "Dataset/Dataset.h"
//test
int main() {
    Dataset dataset;
    const Graph graph = dataset.buildGraph();
    const std::string streetA = "West 55th Street";
    const std::string streetB = "West 61st Street";
    const Edge* e1 = graph.nameToEdge(streetA);
    const Edge* e2 = graph.nameToEdge(streetB);
    //graph.print();
    const double result = graph.Dijkstra(*e1, *e2);

    return 0;
}

// A->C

//A->B ; 1
//B->C: 1
//A->C : 4