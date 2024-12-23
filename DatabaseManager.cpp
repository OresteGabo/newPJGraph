//
// Created by oreste on 23/12/24.
//

#include <QFile>
#include "DatabaseManager.h"
#include "Way.h"

DatabaseManager::DatabaseManager(const QString &host, const QString &dbName, const QString &user, const QString &password) {
    QString connectionName = QString("%1_%2").arg(user, dbName);
    if (QSqlDatabase::contains(connectionName)) {
        db = QSqlDatabase::database(connectionName);
    } else {
        db = QSqlDatabase::addDatabase("QMYSQL", connectionName);
        db.setHostName(host);
        db.setDatabaseName(dbName);
        db.setUserName(user);
        db.setPassword(password);
    }

    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
    } else {
        qDebug() << "Database connection established successfully.";
    }
}

DatabaseManager::~DatabaseManager() {
    QString connectionName = db.connectionName();
    if (db.isOpen()) {
        db.close();
    }
    QSqlDatabase::removeDatabase(connectionName);
}

bool DatabaseManager::initializeDatabase() {
    if (!db.open()) {
        qDebug() << "Failed to connect to MySQL server: " << db.lastError().text();
        return false;
    }

    if (!databaseExists(db.databaseName())) {
        QSqlQuery query(db);
        if (!query.exec("CREATE DATABASE " + db.databaseName())) {
            qDebug() << "Failed to create database: " << query.lastError().text();
            return false;
        }
        qDebug() << "Database created successfully.";
    } else {
        qDebug() << "Database already exists.";
    }

    // Set the database to use after creation or confirmation
    db.setDatabaseName(db.databaseName());
    return createTables();
}

bool DatabaseManager::databaseExists(const QString &dbName) {
    QSqlQuery query(db);
    query.exec("SHOW DATABASES;");
    while (query.next()) {
        if (query.value(0).toString() == dbName) {
            return true;
        }
    }
    return false;
}

bool DatabaseManager::createTables() {
    QString createNodesTable = R"(
        CREATE TABLE IF NOT EXISTS Node (
            id BIGINT UNSIGNED PRIMARY KEY,
            lat DOUBLE NOT NULL,
            lon DOUBLE NOT NULL
        )
    )";

    QString createWaysTable = R"(
        CREATE TABLE IF NOT EXISTS Way (
            id BIGINT UNSIGNED PRIMARY KEY,
            tag ENUM('highway', 'building', 'natural_water', 'natural_green') NOT NULL
        )
    )";

    QString createWayNodesTable = R"(
        CREATE TABLE IF NOT EXISTS WayNodes (
            way_id BIGINT UNSIGNED NOT NULL,
            node_id BIGINT UNSIGNED NOT NULL,
            node_order TINYINT UNSIGNED NOT NULL,
            PRIMARY KEY (way_id, node_id),
            FOREIGN KEY (way_id) REFERENCES Way(id) ON DELETE CASCADE,
            FOREIGN KEY (node_id) REFERENCES Node(id) ON DELETE CASCADE
        )
    )";

    return executeQuery(createNodesTable) &&
           executeQuery(createWaysTable) &&
           executeQuery(createWayNodesTable);
}

bool DatabaseManager::executeQuery(QString queryStr) {
    QSqlQuery query(db);
    if (!query.exec(queryStr)) {
        qDebug() << "SQL Error: " << query.lastError().text();
        return false;
    }
    return true;
}

void DatabaseManager::loadOSMFile(const QString & filePath) {
    if (!QFile::exists(filePath)) {
        qDebug() << "OSM file does not exist: " << filePath;
        return;
    }

    // OSM file parsing logic goes here, inserting into Node, Way, and WayNodes
    // This function serves as a placeholder for OSM data import
}
bool DatabaseManager::insertWay(const Way &way) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO Way (id, tag) VALUES (:id, :tag)");

    // Explicitly cast uint64_t to QVariant-compatible qulonglong
    query.bindValue(":id", QVariant::fromValue(static_cast<qulonglong>(way.getId())));
    query.bindValue(":tag", way.getTag());

    if (!query.exec()) {
        qDebug() << "Failed to insert way:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::insertNodes(const QVector<Node>& nodes) {
    QSqlQuery query(db);
    db.transaction();
    for (const Node& node : nodes) {
        query.prepare("INSERT INTO Node (id, lat, lon) VALUES (:id, :lat, :lon)");
        query.bindValue(":id", QVariant::fromValue(static_cast<qulonglong>(node.getId())));
        query.bindValue(":lat", node.getLatitude());
        query.bindValue(":lon", node.getLongitude());
        if (!query.exec()) {
            qDebug() << "Failed to insert node:" << query.lastError().text();
            db.rollback();
            return false;
        }
    }
    db.commit();
    return true;
}


bool DatabaseManager::insertWayNode(uint64_t wayId, uint64_t nodeId, uint8_t nodeOrder) {
    QString queryStr = QString("INSERT INTO WayNodes (way_id, node_id, node_order) VALUES (%1, %2, %3)")
            .arg(wayId)
            .arg(nodeId)
            .arg(nodeOrder);

    return executeQuery(queryStr);
}
WayType DatabaseManager::determineWayType(const QXmlStreamAttributes &attributes) {
    QString k = attributes.value("k").toString();
    QString v = attributes.value("v").toString();

    if (k == "highway") {
        return WayType::Highway;
    } else if (k == "water" || (k == "natural" && v == "water")) {
        return WayType::NaturalWater;
    } else if ((k == "landuse" && (v == "forest" || v == "grass")) || (k == "natural" && v == "tree_row")) {
        return WayType::NaturalGreen;
    }
    return WayType::Unknown;
}
void DatabaseManager::parseNodes(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file: " << fileName;
        return;
    }

    QXmlStreamReader xml(&file);

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement() && xml.name() == "node") {
            QString id = xml.attributes().value("id").toString();
            double lat = xml.attributes().value("lat").toDouble();
            double lon = xml.attributes().value("lon").toDouble();

            QString queryStr = QString("INSERT INTO Node (id, lat, lon) VALUES (%1, %2, %3)")
                    .arg(id.toULongLong())
                    .arg(lat)
                    .arg(lon);

            if (!executeQuery(queryStr)) {
                qDebug() << "Failed to insert node with ID: " << id;
            }
        }
    }

    if (xml.hasError()) {
        qDebug() << "XML Parsing Error: " << xml.errorString();
    }
    file.close();
}
void DatabaseManager::parseWays(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file: " << fileName;
        return;
    }

    QXmlStreamReader xml(&file);
    bool isWay = false;
    QString wayId;
    QVector<QPair<uint64_t, uint8_t>> wayNodes; // To store nodes for this way
    WayType currentWayType = WayType::Unknown;

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            if (xml.name() == "way") {
                isWay = true;
                wayId = xml.attributes().value("id").toString();
                currentWayType = WayType::Unknown; // Reset type for the new way
                wayNodes.clear(); // Clear previous way nodes
            } else if (isWay && xml.name() == "nd") {
                uint64_t nodeId = xml.attributes().value("ref").toULongLong();
                wayNodes.append({nodeId, static_cast<uint8_t>(wayNodes.size() + 1)});
            } else if (isWay && xml.name() == "tag") {
                WayType tagType = determineWayType(xml.attributes());
                if (tagType != WayType::Unknown) {
                    currentWayType = tagType; // Set type if it matches known tags
                }
            }
        } else if (xml.isEndElement() && xml.name() == "way" && isWay) {
            isWay = false;

            if (currentWayType != WayType::Unknown) {
                QString tag;
                switch (currentWayType) {
                    case WayType::Highway:
                        tag = "highway";
                        break;
                    case WayType::NaturalWater:
                        tag = "natural_water";
                        break;
                    case WayType::NaturalGreen:
                        tag = "natural_green";
                        break;
                    default:
                        tag = "";
                        break;
                }

                // Insert into Way table
                QString wayQuery = QString("INSERT INTO Way (id, tag) VALUES (%1, '%2')")
                        .arg(wayId.toULongLong())
                        .arg(tag);
                if (!executeQuery(wayQuery)) {
                    qDebug() << "Failed to insert way with ID: " << wayId;
                }

                // Insert into WayNodes table
                for (const auto &wayNode : wayNodes) {
                    QString nodeQuery = QString("INSERT INTO WayNodes (way_id, node_id, node_order) VALUES (%1, %2, %3)")
                            .arg(wayId.toULongLong())
                            .arg(wayNode.first)
                            .arg(wayNode.second);
                    if (!executeQuery(nodeQuery)) {
                        qDebug() << "Failed to insert way-node with Way ID: " << wayId
                                 << " and Node ID: " << wayNode.first;
                    }
                }
            }
        }
    }

    if (xml.hasError()) {
        qDebug() << "XML Parsing Error: " << xml.errorString();
    }
    file.close();
}

AdjacencyList DatabaseManager::buildAdjacencyList() {
    AdjacencyList adjList;

    // Step 1: Retrieve WayNodes to build node-to-node connections
    QString wayNodesQuery = R"(
        SELECT w1.node_id AS from_node, w2.node_id AS to_node
        FROM WayNodes w1
        JOIN WayNodes w2
        ON w1.way_id = w2.way_id
        WHERE w1.node_order + 1 = w2.node_order
    )";

    QSqlQuery query(db);
    if (query.exec(wayNodesQuery)) {
        while (query.next()) {
            uint64_t fromNodeId = query.value("from_node").toULongLong();
            uint64_t toNodeId = query.value("to_node").toULongLong();
            adjList.addConnection(fromNodeId, toNodeId);
        }
    } else {
        qDebug() << "Failed to fetch WayNodes:" << query.lastError().text();
    }

    // Step 2: Retrieve Ways and associate them with their starting nodes
    QString waysQuery = R"(
    SELECT Way.id AS way_id,
           MIN(WayNodes.node_order) AS start_order,
           MIN(Node.id) AS start_node
    FROM Way
    JOIN WayNodes ON Way.id = WayNodes.way_id
    JOIN Node ON WayNodes.node_id = Node.id
    GROUP BY Way.id
)";

    if (query.exec(waysQuery)) {
        while (query.next()) {
            uint64_t wayId = query.value("way_id").toULongLong();
            uint64_t startNodeId = query.value("start_node").toULongLong();
            Way way(wayId, query.value("tag").toString());
            adjList.addWay(way, startNodeId);
        }
    } else {
        qDebug() << "Failed to fetch Ways:" << query.lastError().text();
    }

    return adjList;
}

std::tuple<double, double, double, double> DatabaseManager::calculateBounds() {
    if (!db.isOpen()) {
        qDebug() << "Database is not open.";
        return {0.0, 0.0, 0.0, 0.0};
    }

    double minLat = std::numeric_limits<double>::max();
    double maxLat = std::numeric_limits<double>::lowest();
    double minLon = std::numeric_limits<double>::max();
    double maxLon = std::numeric_limits<double>::lowest();

    QSqlQuery query(db);
    if (query.exec("SELECT MIN(lat), MAX(lat), MIN(lon), MAX(lon) FROM Node")) {
        if (query.next()) {
            minLat = query.value(0).toDouble();
            maxLat = query.value(1).toDouble();
            minLon = query.value(2).toDouble();
            maxLon = query.value(3).toDouble();
        }
    } else {
        qDebug() << "Failed to calculate bounds:" << query.lastError().text();
    }

    return {minLat, maxLat, minLon, maxLon};
}

bool DatabaseManager::isDataAvailable() {
    QSqlQuery query(db);
    if (query.exec("SELECT EXISTS(SELECT 1 FROM Node LIMIT 1) OR EXISTS(SELECT 1 FROM Way LIMIT 1)")) {
        if (query.next()) {
            return query.value(0).toBool();
        }
    }
    return false;
}


std::unordered_map<uint64_t, std::vector<uint64_t>> DatabaseManager::fetchWays() {
    std::unordered_map<uint64_t, std::vector<uint64_t>> ways;

    QSqlQuery query(db);
    if (query.exec("SELECT way_id, node_id FROM WayNodes ORDER BY way_id, node_order")) {
        while (query.next()) {
            uint64_t wayId = query.value("way_id").toULongLong();
            uint64_t nodeId = query.value("node_id").toULongLong();

            ways[wayId].push_back(nodeId);
        }
    } else {
        qDebug() << "Failed to fetch ways:" << query.lastError().text();
    }

    return ways;
}


std::vector<Node> DatabaseManager::fetchNodes() {
    std::vector<Node> nodes;

    QSqlQuery query(db);
    if (query.exec("SELECT id, lat, lon FROM Node")) {
        while (query.next()) {
            uint64_t id = query.value("id").toULongLong();
            double lat = query.value("lat").toDouble();
            double lon = query.value("lon").toDouble();

            nodes.emplace_back(id, lat, lon);
        }
    } else {
        qDebug() << "Failed to fetch nodes:" << query.lastError().text();
    }

    return nodes;
}

QString DatabaseManager::getTag(uint64_t wayId)  {
   if (!db.isOpen()) {
        qDebug() << "Database connection is not open in getTag.";
        return QString();
    }

    QSqlQuery query(db);
    query.prepare("SELECT tag FROM Way WHERE id = :wayId");
    query.bindValue(":wayId", QVariant::fromValue(static_cast<qulonglong>(wayId)));

    if (query.exec() && query.next()) {
        return query.value("tag").toString();
    } else {
        qDebug() << "Failed to retrieve tag for Way ID" << wayId << ":" << query.lastError().text();
    }

    return QString(); // Return an empty string if the query fails
}
