#include <random>
#include "AdjacencyList.h"

void AdjacencyList::addConnection(uint64_t fromNodeId, uint64_t toNodeId) {
    adjacencyMap[fromNodeId].push_back(toNodeId);
}


const QVector<uint64_t> AdjacencyList::getConnectedNodes(uint64_t nodeId) const {
    static const std::vector<uint64_t> empty;
    auto it = adjacencyMap.find(nodeId);
    return it != adjacencyMap.end() ? QVector<uint64_t>::fromStdVector(it->second) : QVector<uint64_t>::fromStdVector(empty);
}

void AdjacencyList::addWay(const Way& way, uint64_t startNodeId) {
    wayStartNodes[way.getId()] = startNodeId;
}

uint64_t AdjacencyList::getStartNode(uint64_t wayId) const {
    auto it = wayStartNodes.find(wayId);
    if (it != wayStartNodes.end()) {
        return it->second;
    }
    return 0; // Return 0 if the way ID is not found
}

std::vector<uint64_t> AdjacencyList::getWayIds() const {
    std::vector<uint64_t> wayIds;
    for (const auto& pair : wayStartNodes) {
        wayIds.push_back(pair.first);
    }
    return wayIds;
}

std::vector<uint64_t> AdjacencyList::getConnectedWays(uint64_t wayId) const {
    uint64_t startNode = getStartNode(wayId);
    if (startNode == 0) return {};

    std::vector<uint64_t> connectedWays;
    const auto& connectedNodes = getConnectedNodes(startNode);
    for (uint64_t node : connectedNodes) {
        for (const auto& pair : wayStartNodes) {
            if (pair.second == node && pair.first != wayId) {
                connectedWays.push_back(pair.first);
            }
        }
    }
    return connectedWays;
}

uint64_t AdjacencyList::getRandomWayId() const {
    std::vector<uint64_t> wayIds = getWayIds();
    if (wayIds.empty()) return 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(0, wayIds.size() - 1);

    return wayIds[dis(gen)];
}

bool AdjacencyList::isEmpty() const {
    return adjacencyMap.empty() && wayStartNodes.empty();
}

uint64_t AdjacencyList::getRandomConnectedWay(uint64_t wayId) const {
    // Get the connected ways for the given way ID
    std::vector<uint64_t> connectedWays = getConnectedWays(wayId);

    // If no connected ways exist, return 0 (or another sentinel value)
    if (connectedWays.empty()) {
        return 0;
    }

    // Select a random way ID from the list of connected ways
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(0, connectedWays.size() - 1);

    return connectedWays[dis(gen)];
}

