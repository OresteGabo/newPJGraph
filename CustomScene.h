#ifndef CUSTOMSCENE_H
#define CUSTOMSCENE_H

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <unordered_map>
#include <vector>
#include "Node.h"
#include "Way.h"

class CustomScene : public QGraphicsScene {
Q_OBJECT
public:
    void drawElements(const std::vector<Node> &nodes, const std::unordered_map<uint64_t, std::vector<uint64_t>> &ways,
                      const std::unordered_map<uint64_t, Node> &nodeMap);

public:
    explicit CustomScene(int width = 1920, int height = 1080, QObject *parent = nullptr);

    // Set latitude and longitude bounds for scaling
    void setBounds(double minLat, double maxLat, double minLon, double maxLon);

    // Add nodes to the scene
    void addNodes(const std::vector<Node>& nodes);

    // Add ways to the scene
    void addWays(const std::unordered_map<uint64_t, std::vector<uint64_t>>& ways, const std::unordered_map<uint64_t, Node>& nodeMap);

protected:
    // Override mouse event handlers
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    // Emit when the scene is clicked
    void sceneClicked(const QPointF& point);

private:
    double minLat, maxLat, minLon, maxLon; // Bounds for latitude and longitude
    void drawTestSquare();
};

#endif // CUSTOMSCENE_H
