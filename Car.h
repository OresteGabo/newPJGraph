#ifndef CAR_H
#define CAR_H

#include <QString>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include "AdjacencyList.h"

class Car {
public:
    Car(const QString& id, uint64_t startNodeId, const AdjacencyList& adjList, QGraphicsScene* scene);

    QString getId() const;
    QPointF getCurrentPosition() const;
    uint64_t getCurrentWayId() const;

    void moveAlongWay();
    bool isAtEndOfWay() const;
    void transitionToWay(uint64_t nextWayId, const AdjacencyList& adjList);

private:
    QString id;                     // Car identifier
    uint64_t currentNodeId;         // Current node ID
    uint64_t currentWayId;          // Current way ID
    QPointF currentPosition;        // Current position on the scene
    const AdjacencyList& adjList;   // Reference to the adjacency list
    QGraphicsEllipseItem* graphic; // Representation of the car in the scene

    QVector<uint64_t> path;         // Nodes in the current way (path)
    int currentPathIndex;           // Current index in the path

    void updatePosition();          // Updates the car's position in the scene
};

#endif // CAR_H
