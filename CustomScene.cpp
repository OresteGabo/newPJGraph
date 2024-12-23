#include "CustomScene.h"
#include "DatabaseManager.h"

CustomScene::CustomScene(int width, int height, QObject *parent)
        : QGraphicsScene(parent), minLat(0), maxLat(0), minLon(0), maxLon(0) {
    setSceneRect(0, 0, width, height);
}

void CustomScene::setBounds(double minLat, double maxLat, double minLon, double maxLon) {
    this->minLat = minLat;
    this->maxLat = maxLat;
    this->minLon = minLon;
    this->maxLon = maxLon;
}

void CustomScene::addNodes(const std::vector<Node>& nodes) {
    double width = sceneRect().width();
    double height = sceneRect().height();

    for (const Node& node : nodes) {
        // Scale lat/lon to scene coordinates
        double x = (node.getLongitude() - minLon) / (maxLon - minLon) * width;
        double y = height - (node.getLatitude() - minLat) / (maxLat - minLat) * height; // Flip y-axis

        // Create a small circle to represent the node
        auto ellipse = new QGraphicsEllipseItem(x - 2, y - 2, 4, 4); // Circle size: 4x4
        ellipse->setBrush(Qt::blue);
        ellipse->setPen(Qt::NoPen);
        addItem(ellipse);
    }
}

void CustomScene::addWays(const std::unordered_map<uint64_t, std::vector<uint64_t>>& ways, const std::unordered_map<uint64_t, Node>& nodeMap) {
    double width = sceneRect().width();
    double height = sceneRect().height();

    for (const auto& [wayId, nodeIds] : ways) {
        for (size_t i = 0; i + 1 < nodeIds.size(); ++i) {
            auto startIt = nodeMap.find(nodeIds[i]);
            auto endIt = nodeMap.find(nodeIds[i + 1]);

            if (startIt != nodeMap.end() && endIt != nodeMap.end()) {
                const Node& startNode = startIt->second;
                const Node& endNode = endIt->second;

                // Scale lat/lon to scene coordinates
                double x1 = (startNode.getLongitude() - minLon) / (maxLon - minLon) * width;
                double y1 = height - (startNode.getLatitude() - minLat) / (maxLat - minLat) * height; // Flip y-axis
                double x2 = (endNode.getLongitude() - minLon) / (maxLon - minLon) * width;
                double y2 = height - (endNode.getLatitude() - minLat) / (maxLat - minLat) * height; // Flip y-axis

                // Create a line to represent the way
                QGraphicsLineItem* line = new QGraphicsLineItem(x1, y1, x2, y2);
                line->setPen(QPen(Qt::black, 1)); // Line width: 1
                addItem(line);
            }
        }
    }
}

void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QPointF clickedPoint = event->scenePos();
    emit sceneClicked(clickedPoint);
    QGraphicsScene::mousePressEvent(event);
}

void CustomScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QPointF movedPoint = event->scenePos();
    qDebug() << "Mouse moved to:" << movedPoint;
    QGraphicsScene::mouseMoveEvent(event);
}
void CustomScene::drawElements(const std::vector<Node>& nodes,
                               const std::unordered_map<uint64_t, std::vector<uint64_t>>& ways,
                               const std::unordered_map<uint64_t, Node>& nodeMap) {
    double width = sceneRect().width();
    double height = sceneRect().height();
    //drawTestSquare();

    for (const auto& [wayId, nodeIds] : ways) {
        // Retrieve the way type (e.g., highway, building, natural_water, natural_green)

        DatabaseManager dbManager("127.0.0.1", "ReseauDB", "oreste", "Muhirehonore@1*");
        QString tag;
        if (dbManager.initializeDatabase()) {
            tag = dbManager.getTag(wayId);
            if (!tag.isEmpty()) {
                qDebug() << "Tag for Way ID :" <<wayId<< " is "<< tag;
            } else {
                qDebug() << "Failed to retrieve the tag for Way ID 12345.";
            }
        }
        //QString tag=getTag(wayId); // Assume this is retrieved from the database or another source

        if (nodeIds.size() < 2) continue; // Skip ways with less than two nodes

        QPolygonF polygon; // For areas (buildings, water, natural_green)
        QVector<QPointF> polyline; // For roads (highways)

        for (uint64_t nodeId : nodeIds) {
            auto it = nodeMap.find(nodeId);
            if (it != nodeMap.end()) {
                const Node& node = it->second;
                double x = (node.getLongitude() - minLon) / (maxLon - minLon) * width;
                double y = height - (node.getLatitude() - minLat) / (maxLat - minLat) * height; // Flip y-axis

                if (tag == "highway") {
                    polyline.append(QPointF(x, y));
                } else {
                    polygon << QPointF(x, y);
                }
            }
        }

        // Draw elements based on their type
        if (tag == "highway") {
            // Draw road as a polyline
            for (int i = 0; i < polyline.size() - 1; ++i) {
                QGraphicsLineItem* line = new QGraphicsLineItem(polyline[i].x(), polyline[i].y(),
                                                                polyline[i + 1].x(), polyline[i + 1].y());
                line->setPen(QPen(Qt::gray, 2, Qt::DashLine)); // Simulate road with dashed gray lines
                addItem(line);
            }
        } else if (tag == "building") {
            // Draw buildings as filled polygons
            auto building = new QGraphicsPolygonItem(polygon);
            building->setBrush(Qt::lightGray);
            building->setPen(Qt::NoPen);
            addItem(building);
        } else if (tag == "natural_water") {
            // Draw water areas
            auto water = new QGraphicsPolygonItem(polygon);
            water->setBrush(Qt::blue);
            water->setPen(Qt::NoPen);
            addItem(water);
        } else if (tag == "natural_green") {
            // Draw green areas
            auto greenArea = new QGraphicsPolygonItem(polygon);
            greenArea->setBrush(Qt::green);
            greenArea->setPen(Qt::NoPen);
            addItem(greenArea);
        }
    }
}
void CustomScene::drawTestSquare() {
    // Clear the scene to avoid overlapping test items
    clear();

    // Define square coordinates
    double width = sceneRect().width();
    double height = sceneRect().height();

    // Create a simple square in the center of the scene
    QGraphicsRectItem* square = new QGraphicsRectItem(width / 4, height / 4, width / 2, height / 2);
    square->setBrush(Qt::red); // Fill with red color
    square->setPen(QPen(Qt::black, 2)); // Black border with width 2
    addItem(square);

    // Add some text inside the square
    QGraphicsTextItem* text = new QGraphicsTextItem("Test Square");
    text->setDefaultTextColor(Qt::white);
    text->setFont(QFont("Arial", 14, QFont::Bold));
    text->setPos(width / 4 + 10, height / 4 + 10);
    addItem(text);

    qDebug() << "Test square drawn on the scene.";
}
