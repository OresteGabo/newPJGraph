//
// Created by oreste on 23/12/24.
//



#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDebug>

class CustomGraphicsView : public QGraphicsView {
Q_OBJECT

public:
    explicit CustomGraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool isPanning;
    QPoint panStartPoint;
};

#endif // CUSTOMGRAPHICSVIEW_H

