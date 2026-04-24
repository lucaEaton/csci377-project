#include <cstdio>
#include "crow.h"
#include <iostream>
#include <unordered_set>
#include "Graph_Architecture/Graph.h"
#include "Dataset/Dataset.h"
#include <mutex>
#include "crow/middlewares/cors.h"

int main() {
    std::unordered_set<std::string> streetNames;
    std::mutex graph_mutex;
    crow::App<crow::CORSHandler> app;

    Dataset dataset;
    auto graph = std::make_unique<Graph>(std::move(dataset.buildGraph()));
    for (const auto &edges: graph->getEdges() | std::views::values) {
        streetNames.emplace(edges->getStreetName());
    }
    //graph->print();

    auto &cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
            .origin("http://localhost:5173")
            .methods("GET"_method);

    CROW_ROUTE(app, "/streets")([&streetNames](){
        nlohmann::json res;
        res["availableStreets"] = nlohmann::json::array();
        for (const auto &st : streetNames) res["availableStreets"].push_back(st);
        crow::response r(200, res.dump());
        r.set_header("Content-Type", "application/json");
        r.set_header("Access-Control-Allow-Origin", "http://localhost:5173");
        return r;
    });

    CROW_ROUTE(app, "/dijkstra")([&graph, &graph_mutex](const crow::request &req) {
        if (!graph) return crow::response(400, "Graph not loaded");
        const auto sA = req.url_params.get("streeta");
        const auto sB = req.url_params.get("streetb");
        std::lock_guard<std::mutex> lock(graph_mutex);
        const auto [travelTime, path, runTime] = graph->Dijkstra(sA, sB);
        if (travelTime < 0) return crow::response(404, "No path found");

        nlohmann::json res;
        res["travelTime"] = travelTime;
        res["path"] = nlohmann::json::array();
        res["runTime"] = runTime;
        // convert our result.path to now a json array
        for (const auto &[lat, lon]: path) res["path"].push_back({{"lat", lat}, {"lon", lon}});

        crow::response r(200, res.dump());
        r.add_header("Content-Type", "application/json");
        return r;
    });

    CROW_ROUTE(app, "/aStar")([&graph, &graph_mutex](const crow::request &req) {
        if (!graph) return crow::response(400, "Graph not loaded");
        const auto sA = req.url_params.get("streeta");
        const auto sB = req.url_params.get("streetb");
        std::lock_guard<std::mutex> lock(graph_mutex);
        const auto [travelTime, path, runTime] = graph->AStar(sA, sB);
        if (travelTime < 0) return crow::response(404, "No path found");

        nlohmann::json res;
        res["travelTime"] = travelTime;
        res["path"] = nlohmann::json::array();
        res["runTime"] = runTime;
        // convert our result.path to now a json array
        for (const auto &[lat, lon]: path) res["path"].push_back({{"lat", lat}, {"lon", lon}});

        crow::response r(200, res.dump());
        r.add_header("Content-Type", "application/json");
        return r;
    });

    CROW_ROUTE(app, "/bellmanFord")([&graph, &graph_mutex](const crow::request &req) {
        if (!graph) return crow::response(400, "Graph not loaded");
        const auto sA = req.url_params.get("streeta");
        const auto sB = req.url_params.get("streetb");
        std::lock_guard<std::mutex> lock(graph_mutex);
        const auto [travelTime, path, runTime] = graph->Bellman_Ford(sA, sB);
        if (travelTime < 0) return crow::response(404, "No path found");

        nlohmann::json res;
        res["travelTime"] = travelTime;
        res["path"] = nlohmann::json::array();
        res["runTime"] = runTime;
        // convert our result.path to now a json array
        for (const auto &[lat, lon]: path) res["path"].push_back({{"lat", lat}, {"lon", lon}});

        crow::response r(200, res.dump());
        r.add_header("Content-Type", "application/json");
        return r;
    });

    app.port(8000).multithreaded().run();
}
