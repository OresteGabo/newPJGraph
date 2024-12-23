//
// Created by oreste on 23/12/24.
//

#include "Car.h"

Car::Car(const QString& id, uint64_t startNodeId, const AdjacencyList& adjList, QGraphicsScene* scene)
        : id(id), currentNodeId(startNodeId), adjList(adjList), currentPathIndex(0) {
    // Initialize graphical representation
    graphic = new QGraphicsEllipseItem(-5, -5, 10, 10); // Small circle representing the car
    graphic->setBrush(Qt::red);
    scene->addItem(graphic);

    // Set initial position
    currentPosition = QPointF(0, 0); // Default, should be updated to the node's position
    graphic->setPos(currentPosition);

    // Load path for the current way
    if (!adjList.getConnectedNodes(startNodeId).empty()) {
        path = adjList.getConnectedNodes(startNodeId);
        if (!path.empty()) {
            currentPathIndex = 0;
            updatePosition();
        }
    }
}

QString Car::getId() const {
    return id;
}

QPointF Car::getCurrentPosition() const {
    return currentPosition;
}

uint64_t Car::getCurrentWayId() const {
    return currentWayId;
}

void Car::moveAlongWay() {
    if (currentPathIndex + 1 < path.size()) {
        currentPathIndex++;
        updatePosition();
    }
}

bool Car::isAtEndOfWay() const {
    return currentPathIndex + 1 >= path.size();
}

void Car::transitionToWay(uint64_t nextWayId, const AdjacencyList& adjList) {
    currentWayId = nextWayId;

    // Load the new path
    uint64_t startNodeId = adjList.getStartNode(nextWayId);
    path = adjList.getConnectedNodes(startNodeId);

    if (!path.empty()) {
        currentPathIndex = 0;
        updatePosition();
    }
}

void Car::updatePosition() {
    if (currentPathIndex < path.size()) {
        uint64_t nextNodeId = path[currentPathIndex];
        // In a real implementation, convert node ID to geographic coordinates
        // Here, we'll just simulate movement in the scene

        currentPosition = QPointF(nextNodeId * 10, nextNodeId * 10); // Dummy logic for visualization
        graphic->setPos(currentPosition);
    }
}

