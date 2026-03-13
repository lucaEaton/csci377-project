//
// Created by luca eaton on 3/11/26.
//
#include "Dataset.h"
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "../Graph_Architecture/Graph.h"
#include "../Graph_Architecture/Vertex.h"
/**
 * Sourced from StackOverflow, will allow me to store the JSON data within a string, to further than parse through the data.
 * O(N) Space complexity.
 *
 * We could have this write to a JSON file allowing for O(1) space complexity
 * within the program and have it stored directly on the disk.
*
 * @param contents
 *   Pointer to the raw data buffer received from libcurl.
 *
 * @param size
 *   Size in bytes of a single data element.
 *
 * @param nmemb
 *   Number of data elements received in this callback invocation.
 *   The total number of bytes received is `size * nmemb`.
 *
 * @param userp
 *   User-provided pointer passed via CURLOPT_WRITEDATA.
 *   In this implementation it is expected to be a pointer to a
 *   std::string where the response data will be appended.
 *
 * @return
 *   The number of bytes successfully processed. libcurl expects
 *   this to equal `size * nmemb`. Returning a smaller value signals
 *   an error and aborts the transfer.
 */
size_t Dataset::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    static_cast<string*>(userp)->append(static_cast<char *>(contents), size * nmemb);
    return size * nmemb;
}
/**
 * Sends a request to the overpass api to query road data from
 * OSM(OpenStreetMap) near a specified location.
 *
 * libcurl is used to send the request. The JSON response from the
 * API is collected using the WriteCallback function and stored in
 * a temporary string. After the request finishes, the data is saved
 * into the class's private variable `jsonData`.
 *
 * Time Complex : O(N) n = size of res
 * Space Complex : O(N) due to storing the response within a string 'jsonData'.
 */
void Dataset::overseeAPI() {
    CURL *curl = curl_easy_init();
    //raw string query
    const string q = R"([out:json][timeout:25];(way["highway"~"^(motorway|trunk|primary|secondary|tertiary|residential|unclassified|living_street)$"](around:300,40.7692,-73.9866);node(w););out body qt;)";
    //storing the query for us to "point" curl back to it.
    const string data = "data=" + q;
    curl_easy_setopt(curl, CURLOPT_URL,  "https://overpass-api.de/api/interpreter"); //set domain
    curl_easy_setopt(curl, CURLOPT_POST, 1L); //GET Request instead POST request
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str()); //make query, create a c pointer to allow curl to be able to fully reread the query
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Dataset::WriteCallback); // store json
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &jsonData); // stores json in std::string (jsonData);
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // debug usage
    //preform the act request
    CURLcode res = curl_easy_perform(curl);
    //erase any allocated mem we had set to avoid any mem leaks
    if (res != CURLE_OK) {
        //debug
        std::cerr << "failed request -  " << curl_easy_strerror(res) << "\n";
    }
    curl_easy_cleanup(curl);
    //std::cout << jsonData << std::endl; // testing to see if its saved
}
/*
 * Done to find the distance between points(lat n long) using the Haversine function
 * due to the oversees api not providing the distance
 *
 * Sourced from GeeksForGeeks
 * : https://www.geeksforgeeks.org/dsa/haversine-formula-to-find-distance-between-two-points-on-a-sphere/
 */
double deg2rad(double deg) {return (deg * M_PI / 180.0);}
double haversine(const double lat1d, const double lon1d, const double lat2d, const double lon2d) {
    constexpr double r = 6371.0; // earth radius in KM
    const double lat1 = deg2rad(lat1d);
    const double lon1 = deg2rad(lon1d);
    const double lat2 = deg2rad(lat2d);
    const double lon2 = deg2rad(lon2d);
    const double dlat = lat2 - lat1, dlon = lon2 - lon1;
    const double a = std::pow(std::sin(dlat / 2), 2) +
               std::cos(lat1) * std::cos(lat2) *
               std::pow(std::sin(dlon / 2), 2);
    return 2 * r * std::asin(std::sqrt(a));
}
// parse data
 Graph Dataset::parseData() {
    using json = nlohmann::json;
    Graph graph;
    for (auto roadData = json::parse(jsonData); const auto& e : roadData["elements"]) {
        //Within the json file, if the type is = to 'node', then it can be saved n parsed as a vertex object
        if (e["type"] == "node") {
            //gather id, latitude and longitude of the node.
            const long long nodeID = e["id"];
            const double lat = (e["lat"].get<double>());
            const double lng = (e["lon"].get<double>());
            graph.addVertx(nodeID, lat, lng);
        }
        //Within the json file, if the type is = to 'way', then it can be saved n parsed as an edge object
        if (e["type"] == "way") {
            const long long edgeID= e["id"];
            const auto& t = e["tags"];
            string name = t["name"];
            //For some reason the maxspeed or speed limit isn't displayed, but I assume if it's not listed, its 25 according to nyc law.
            double speed = 25;
            if (t.contains("maxspeed")) speed = std::stod(t["maxspeed"].get<std::string>());
            const auto& u = e["nodes"];
            for (size_t i = 0; i+1<u.size(); ++i) {
                /*
                 * sliding window : gather nodes in pairs of 2 to create edges
                 * as a street may hold more than one node.
                 */
                Vertex *srcNode = graph.getVertex((u[i].get<long long>()));
                Vertex *destNode = graph.getVertex((u[i + 1].get<long long>()));
                //calc distance between 2 nodes
                const double dist = haversine(srcNode->getLat(), srcNode->getLon(), destNode->getLat(), destNode->getLon());
                graph.addEdge(edgeID,srcNode,destNode,dist,speed,name);
            }
        }
    }
    //clear memory of the json file
    jsonData.clear();
    jsonData.shrink_to_fit();
    std::cout<< "loaded graph\n" << std::endl;
    return graph;
 }
/*
 * wrapped calls
 */
Graph Dataset::buildGraph() {
    overseeAPI();
    return parseData();
}