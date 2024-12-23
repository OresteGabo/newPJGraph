//
// Created by oreste on 23/12/24.
//

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QFileDialog>
#include <QRandomGenerator>
#include <QDebug>
#include "CustomGraphicsView.h"
#include "CustomScene.h"
#include "DatabaseManager.h"
#include "Car.h"
#include "AdjacencyList.h"

class MainWidget : public QWidget {
Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);

private slots:
    void clearDebugText();
    void onDisplayInfo();
    void changeData();
    void addCars();
    void onRunButtonClicked();
    void updateAnimation();

private:
    void updateCarPositions(qreal elapsedTime);
    void updateConnectedCars();

    QTextEdit *debugTextArea;
    QPushButton *clearButton;
    QPushButton *runButton;
    QPushButton *addCarButton;
    QPushButton *displayInfo;
    QLineEdit *carInput;
    QGraphicsView *graphicsView;
    QTimer *animationTimer;

    std::vector<Car*> cars;
    AdjacencyList adjList;

    void initializeData();
    void displayDataFromDatabase();
    static void loadDataFromFile(const QString& filePath);
};

#endif // MAINWIDGET_H

