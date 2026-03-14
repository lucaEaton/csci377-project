#include "Dataset.h"
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "../Graph_Architecture/Graph.h"
#include "../Graph_Architecture/Vertex.h"
/**
 * @note
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
 * @timecomplex O(N) n = size of res
 * @spacecomplex O(N) due to storing the response within a string 'jsonData'.
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
 * @note
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
/**
 * @note
 * pares though the json date provided by @see Dataset::overseeAPI().
 * The expected input format is as follows:
 * @code{.json}
 * {
 *   "elements": [
 *     {
 *       "type": "node",
 *       "id": 42435710,
 *       "lat": 40.7654396,
 *       "lon": -73.9838209,
 *       "tags": {
 *         "highway": "traffic_signals",
 *         "traffic_signals": "signal"
 *       }
 *     },
 *     {
 *       "type": "way",
 *       "id": 195743297,
 *       "nodes": [ 42455643, 3998699938, 12427653344 ],
 *       "tags": {
 *         "highway": "primary",
 *         "maxspeed": "25 mph",
 *         "name": "10th Avenue",
 *         "oneway": "yes"
 *       }
 *     }
 *   ]
 * }
 * @endcode
 *
 * we loop through every array value within 'elements'
 * we perform three separate runs through of this object
 *
 * @run1
 * the first run is done in ordered to allocate specific memory for the graph itself
 * every string value that holds the name 'node' (vertex) or 'way' (edge) we add to their respected counters
 * we then take those values in order to persevere space for each data structure
 * the cost without reserving is @timecompelx O(N log N) total copy operations across all reallocations
 * with reserve, it's @timecomplex O(N) — each element written exactly once
 * this is vital as wwe could have thousands of nodes
 * @see lines 157-165
 *
 * @run2
 * we are now hunting for our vertices, we cannot build our edges without them so its vital that we do so.
 * in how our json file is formatted, it will put our nodes first to avoid this issue, however I'm not sure if
 * that's always promised so avoiding that test case we perform this run anyway.
 * if we can for certain know that it will always come first we are able to avoid this redundant for loop
 * and combine @run2 and @run3
 * @see lines 169-177
 *
 * @run3
 * our final for loop will now target array objects with its type being 'way' so that we may build our edges
 * we store variables that we will need in order to create our edge object
 * our distance isn't provided with our json data, so utilizing the haversine formula to find the distance between
 * two latitude and longitude points can give us an estimate of the distance of our street
 * in test cases where our 'maxspeed' isn't provided, its known though if it isn't, its 25 mph
 * All 'way' arrays hold a 'nodes' arrays so we loop over it in a makeshift sliding window,
 * with ith node being the source vertex and the i+1th node being the destination vertex
 * Add the edge objects to the edge map and street map
 * @see 191-214
 *
 * @endruns
 *
 * Clear the jsonData in order to free up memory
 *
 * @relates Graph
 * @timecomplex O(N+W+E) - N = total elements, W = ways (road segments), E = total edges
 * @return Graph Object
 */
 Graph Dataset::parseData() {
    using json = nlohmann::json;
    auto roadData = json::parse(jsonData);

    //count instances seen in order to allocate memory.
    size_t nodeCount = 0, edgeCount = 0;
    for (const auto& e : roadData["elements"]) {
        if (e["type"] == "node") nodeCount++;
        if (e["type"] == "way") edgeCount += e["nodes"].size() - 1;
    }
    std::cout << "#nodes: " << nodeCount << std::endl;
    std::cout << "#edges: " << edgeCount << std::endl;
    Graph graph(nodeCount, edgeCount);    //create graph object with allocated memory

    //within the json file, if the type is = to 'node', then it can be saved n parsed as a vertex object
    for ( const auto& e : roadData["elements"]) {
        if (e["type"] == "node") {
            //gather id, latitude and longitude of the node.
            const long long nodeID = e["id"];
            const double lat = (e["lat"].get<double>());
            const double lng = (e["lon"].get<double>());
            graph.addVertx(nodeID, lat, lng);
        }
    }

    //within the json file, if the type is = to 'way', then it can be saved n parsed as an edge object
    for ( const auto& e : roadData["elements"]) {
        if (e["type"] == "way") {
            const long long edgeID= e["id"];
            const auto& t = e["tags"];
            const string name = t.value("name", "unknown"); //catch incase name tag is missing
            //For some reason the maxspeed or speed limit isn't displayed, but I assume if it's not listed, its 25 according to nyc law.
            double speed = 25 * 1.60934;  // default 25mph to 40.23 km/h
            if (t.contains("maxspeed")) speed = std::stod(t["maxspeed"].get<std::string>()) * 1.60934;
            const auto& u = e["nodes"];
            for (size_t i = 0; i+1<u.size(); ++i) {
                const long long segmentID = edgeID * 100 + static_cast<long long>(i); // unique per segment

                /*
                 * sliding window : gather nodes in pairs of 2 to create edges
                 * as a street may hold more than one node.
                 */
                Vertex *srcNode = graph.getVertex((u[i].get<long long>()));
                Vertex *destNode = graph.getVertex((u[i + 1].get<long long>()));
                //calc distance between 2 nodes
                const double dist = haversine(srcNode->getLat(), srcNode->getLon(), destNode->getLat(), destNode->getLon());

                graph.addStreet(segmentID,name);
                graph.addEdge(segmentID,srcNode,destNode,dist,speed,name);
            }
        }
    }

    //clear memory of the json file
    jsonData.clear();
    jsonData.shrink_to_fit();
    std::cout<< "successfully loaded\n" << std::endl;
    return graph;
 }
/*
 * @note
 * wrapped calls
 *
 * @timecomplex O(N)
 */
Graph Dataset::buildGraph() {
    overseeAPI();
    return parseData();
}