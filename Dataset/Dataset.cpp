//
// Created by luca eaton on 3/11/26.
//

#include "Dataset.h"
#include <iostream>
#include <string>
#include <curl/curl.h>

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
    static_cast<string *>(userp)->append(static_cast<char *>(contents), size * nmemb);
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

    //Raw string query
    const string q = R"(
                    [out:json][timeout:25];
                    (
                      way["highway"]
                      (around:300,40.7692,-73.9866);
                    );
                    out body;
                    >;
                    out skel qt;
                    )";

    //storing the query for us to "point" curl back to it.
    const string data = "data=" + q;

    curl_easy_setopt(curl, CURLOPT_URL,  "https://overpass-api.de/api/interpreter"); //set domain
    curl_easy_setopt(curl, CURLOPT_POST, 1L); //GET Request instead POST request
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str()); //make query, create a c pointer to allow curl to be able to fully reread the query
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Dataset::WriteCallback); // store json

    string res;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &res); // stores json in std::string (res);

    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // debug usage

    //preform the act request
    curl_easy_perform(curl);
    //save response to mem
    jsonData = res;
    //erase any allocated mem we had set to avoid any mem leaks
    curl_easy_cleanup(curl);

    //std::cout << res << std::endl; // test
}