//
// Created by oreste on 23/12/24.
//

#include <QIntValidator>
#include "MainWidget.h"
#include "MainWidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent) {
    // Initialize scene and view
    auto scene = new CustomScene();
    graphicsView = new CustomGraphicsView(scene, this);

    // Initialize debug text area
    debugTextArea = new QTextEdit();
    debugTextArea->setReadOnly(true);

    // Initialize buttons and layouts
    clearButton = new QPushButton("Clear");
    runButton = new QPushButton("Start Simulation");
    addCarButton = new QPushButton("Add Cars");
    displayInfo = new QPushButton("Info");

    carInput = new QLineEdit();
    carInput->setPlaceholderText("Number of Cars");
    carInput->setValidator(new QIntValidator(1, 100, this));

    connect(clearButton, &QPushButton::clicked, this, &MainWidget::clearDebugText);
    connect(runButton, &QPushButton::clicked, this, &MainWidget::onRunButtonClicked);
    connect(addCarButton, &QPushButton::clicked, this, &MainWidget::addCars);
    connect(displayInfo, &QPushButton::clicked, this, &MainWidget::onDisplayInfo);

    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(clearButton);
    buttonLayout->addWidget(runButton);
    buttonLayout->addWidget(displayInfo);

    auto carInputLayout = new QHBoxLayout();
    carInputLayout->addWidget(carInput);
    carInputLayout->addWidget(addCarButton);

    auto debugLayout = new QVBoxLayout();
    debugLayout->addWidget(debugTextArea, 1);
    debugLayout->addLayout(carInputLayout);
    debugLayout->addLayout(buttonLayout);

    auto mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(graphicsView, 3);
    mainLayout->addLayout(debugLayout, 1);

    setLayout(mainLayout);

    // Animation timer
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &MainWidget::updateAnimation);

    // Initialize data automatically
    initializeData();
}


void MainWidget::clearDebugText() {
    debugTextArea->clear();
}

void MainWidget::onDisplayInfo() {
    debugTextArea->append("Displaying car information:");
    for (const auto &car : cars) {
        debugTextArea->append("Car ID: " + car->getId() +
                              ", Position: (" + QString::number(car->getCurrentPosition().x()) +
                              ", " + QString::number(car->getCurrentPosition().y()) + ")");
    }
}

void MainWidget::changeData() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open OSM File", "", "OSM Files (*.osm)");
    if (!filePath.isEmpty()) {
        DatabaseManager dbManager("OSMData");
        dbManager.loadOSMFile(filePath);
        debugTextArea->append("Loaded OSM file: " + filePath);
        adjList = dbManager.buildAdjacencyList();
    }
}

void MainWidget::addCars() {
    int carCount = carInput->text().toInt();
    if (carCount <= 0) {
        debugTextArea->append("Please enter a valid number of cars.");
        return;
    }

    if (adjList.isEmpty()) {
        debugTextArea->append("No ways available for car placement.");
        return;
    }

    for (int i = 0; i < carCount; ++i) {
        uint32_t wayId = adjList.getRandomWayId();
        uint32_t startNodeId = adjList.getStartNode(wayId);

        QString carId = QString::number(cars.size() + 1);
        auto newCar = new Car(carId, startNodeId, adjList, graphicsView->scene());
        cars.push_back(newCar);

        debugTextArea->append("Added Car ID: " + carId + " on Way ID: " + QString::number(wayId));
    }
}

void MainWidget::onRunButtonClicked() {
    if (animationTimer->isActive()) {
        animationTimer->stop();
        runButton->setText("Start Simulation");
        debugTextArea->append("Simulation stopped.");
    } else {
        animationTimer->start(16);  // Run at ~60 FPS
        runButton->setText("Stop Simulation");
        debugTextArea->append("Simulation started.");
    }
}

void MainWidget::updateAnimation() {
    for (auto &car : cars) {
        car->moveAlongWay();
        if (car->isAtEndOfWay()) {
            uint32_t nextWayId = adjList.getRandomConnectedWay(car->getCurrentWayId());
            if (nextWayId != 0) {
                car->transitionToWay(nextWayId, adjList);
                debugTextArea->append("Car ID: " + car->getId() + " moved to Way ID: " + QString::number(nextWayId));
            }
        }
    }
    update();
}
void MainWidget::initializeData() {
    DatabaseManager dbManager;

    if (dbManager.initializeDatabase()) {
        if (dbManager.isDataAvailable()) {
            qDebug() << "Data available in database. Displaying data.";
            adjList = dbManager.buildAdjacencyList();
            displayDataFromDatabase();
        } else {
            qDebug() << "No data found in database. Loading from file.";
            QString filePath = "map.osm"; // Specify the file path
            loadDataFromFile(filePath);
            adjList = dbManager.buildAdjacencyList();
            displayDataFromDatabase();
        }
    } else {
        qDebug() << "Failed to initialize database.";
    }
}
void MainWidget::displayDataFromDatabase() {
    DatabaseManager dbManager;

    // Calculate bounds
    auto [minLat, maxLat, minLon, maxLon] = dbManager.calculateBounds();

    // Fetch data
    std::vector<Node> nodes = dbManager.fetchNodes();
    auto ways = dbManager.fetchWays();

    // Create a map of nodes for quick lookup
    std::unordered_map<uint64_t, Node> nodeMap;
    for (const Node& node : nodes) {
        nodeMap[node.getId()] = node;
    }

    // Initialize scene bounds
    auto scene = dynamic_cast<CustomScene*>(graphicsView->scene());
    if (!scene) {
        qDebug() << "Failed to cast graphicsView->scene() to CustomScene.";
        return;
    }
    scene->setBounds(minLat, maxLat, minLon, maxLon);

    // Draw elements
    scene->drawElements(nodes, ways, nodeMap);
}



void MainWidget::loadDataFromFile(const QString& filePath) {
    if (!QFile::exists(filePath)) {
        qDebug() << "File does not exist:" << filePath;
        return;
    }

    DatabaseManager dbManager;
    dbManager.parseNodes(filePath);
    dbManager.parseWays(filePath);

    qDebug() << "Data loaded from file into database.";
}