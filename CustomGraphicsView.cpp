//
// Created by oreste on 23/12/24.
//

#include "CustomGraphicsView.h"

CustomGraphicsView::CustomGraphicsView(QGraphicsScene *scene, QWidget *parent)
        : QGraphicsView(scene, parent), isPanning(false) {
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setDragMode(NoDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTransformationAnchor(AnchorUnderMouse);
}

void CustomGraphicsView::wheelEvent(QWheelEvent *event) {
    const double scaleFactor = 1.15;
    if (event->angleDelta().y() > 0) {
        scale(scaleFactor, scaleFactor); // Zoom in
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor); // Zoom out
    }
}

void CustomGraphicsView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        isPanning = true;
        panStartPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent *event) {
    if (isPanning) {
        QPointF delta = mapToScene(event->pos()) - mapToScene(panStartPoint);
        panStartPoint = event->pos();
        setSceneRect(sceneRect().translated(-delta.x(), -delta.y()));
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void CustomGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        isPanning = false;
        setCursor(Qt::ArrowCursor);
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}
