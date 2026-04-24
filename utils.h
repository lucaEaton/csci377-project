// utils.h
#pragma once
#include <cmath>

/*
 * @note
 * Done to find the distance between points(lat n long) using the Haversine function
 * due to the oversees api not providing the distance
 *
 * Sourced from GeeksForGeeks
 * : https://www.geeksforgeeks.org/dsa/haversine-formula-to-find-distance-between-two-points-on-a-sphere/
 */
inline double deg2rad(double deg) { return (deg * M_PI / 180.0); }

inline double haversine(const double lat1d, const double lon1d, const double lat2d, const double lon2d) {
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