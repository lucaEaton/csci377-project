#include "Graph_Architecture/Graph.h"
#include "Dataset/Dataset.h"
//test
int main(void) {
    Dataset dataset;
    Graph graph = dataset.buildGraph();

    graph.print();
    return 0;
}