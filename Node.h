//
// Created by oreste on 23/12/24.
//

#ifndef NEWPJGRAPH_NODE_H
#define NEWPJGRAPH_NODE_H


#include <cstdint>

class Node {
public:
    Node() : id(0), lat(0.0), lon(0.0) {}
    Node(uint64_t id, double lat, double lon)
            : id(id), lat(lat), lon(lon) {}

    uint64_t getId() const { return id; }
    double getLatitude() const { return lat; }
    double getLongitude() const { return lon; }

private:
    uint64_t id;
    double lat;
    double lon;
};




#endif //NEWPJGRAPH_NODE_H
