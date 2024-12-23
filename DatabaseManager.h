//
// Created by oreste on 23/12/24.
//

#ifndef NEWPJGRAPH_DATABASEMANAGER_H
#define NEWPJGRAPH_DATABASEMANAGER_H

#include <string>
#include <vector>
#include <QtSql/QSqlDatabase>
#include <QDebug>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QXmlStreamAttributes>
#include "Way.h"
#include "WayType.h"
#include "AdjacencyList.h"

class DatabaseManager {
public:
    explicit DatabaseManager(const QString &host = "127.0.0.1",
                             const QString &dbName = "ReseauDB",
                             const QString &user = "oreste",
                             const QString &password = "Muhirehonore@1*");

    bool initializeDatabase();
    ~DatabaseManager();
    bool createTables();

    bool databaseExists(const QString& dbName);
    void loadOSMFile(const QString & filePath);
    void insertNode(uint32_t id, double lat, double lon);
    void insertWay(uint32_t id, const std::string& tag);
    bool insertWay(const Way &way);
    bool insertWayNode(uint64_t wayId, uint64_t nodeId, uint8_t nodeOrder);
    void parseNodes(const QString &fileName);
    void parseWays(const QString &fileName);
    AdjacencyList buildAdjacencyList();

    std::tuple<double, double, double, double> calculateBounds();

    std::vector<Node> fetchNodes();

    std::unordered_map<uint64_t, std::vector<uint64_t>> fetchWays();

    bool isDataAvailable();
    QString getTag(uint64_t wayId) ;

private:
    QSqlDatabase db;
    bool executeQuery(QString queryStr);
    WayType determineWayType(const QXmlStreamAttributes &attributes);

    bool insertNodes(const QVector<Node> &nodes);
};


#endif //NEWPJGRAPH_DATABASEMANAGER_H
