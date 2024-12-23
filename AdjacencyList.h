#ifndef ADJACENCYLIST_H
#define ADJACENCYLIST_H

#include <unordered_map>
#include <vector>
#include <cstdint>
#include <QString>
#include <QDebug>
#include "Node.h"
#include "Way.h"

class AdjacencyList {
public:
    AdjacencyList() = default;

    // Add a connection between two nodes
    void addConnection(uint64_t fromNodeId, uint64_t toNodeId);

    // Get all connected nodes for a given node
    const QVector<unsigned long> getConnectedNodes(uint64_t nodeId) const;

    // Add a way and its starting node to the list
    void addWay(const Way& way, uint64_t startNodeId);


    // Get the starting node of a way
    uint64_t getStartNode(uint64_t wayId) const;

    // Get all way IDs
    std::vector<uint64_t> getWayIds() const;

    // Get connected ways for a given way
    std::vector<uint64_t> getConnectedWays(uint64_t wayId) const;

    // Get a random way ID
    uint64_t getRandomWayId() const;

    // Check if the adjacency list is empty
    bool isEmpty() const;

    uint64_t getRandomConnectedWay(uint64_t wayId) const;

private:
    std::unordered_map<uint64_t, std::vector<uint64_t>> adjacencyMap; // Maps nodes to connected nodes
    std::unordered_map<uint64_t, uint64_t> wayStartNodes;             // Maps way ID to start node ID
};

#endif // ADJACENCYLIST_H
