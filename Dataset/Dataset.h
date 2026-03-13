//
// Created by luca eaton on 3/11/26.
//

#ifndef DATASET_H
#define DATASET_H
#include <string>
#include "../Graph_Architecture/Graph.h"
using namespace std;


class Dataset {
    string jsonData;
    public:
        static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
        void overseeAPI();
        Graph parseData();

    Graph buildGraph();
};



#endif //DATASET_H
