#include <QApplication>
#include <QPushButton>
#include <QApplication>
#include <QPushButton>
#include <QDebug>
#include "MainWidget.h"
#include "DatabaseManager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Initialize database manager
    /*DatabaseManager dbManager;
    if (!dbManager.initializeDatabase()) {
        qDebug() << "Failed to initialize the database. Exiting...";
        return -1;
    }

    // Optionally load data from OSM file (if required)
    QString osmFile = "map.osm"; // Path to your OSM file
    dbManager.parseNodes(osmFile);
    dbManager.parseWays(osmFile);
*/
    // Create the main GUI widget
    MainWidget mainWidget;
    mainWidget.resize(1920, 1080); // Set a default size for the window
    mainWidget.show();

    return app.exec();
}
/*
class Node{
    uint32_t id;
    double lat,lon;
public:
    Node(uint32_t id) : id(id) {}
};
enum class WayType {
    Highway,
    Building,
    NaturalWater,
    NaturalGreen
};

std::string toString(WayType type) {
    switch (type) {
        case WayType::Highway: return "highway";
        case WayType::Building: return "building";
        case WayType::NaturalWater: return "natural_water";
        case WayType::NaturalGreen: return "natural";
        default: return "unknown";
    }
}
class Way{

};
 */