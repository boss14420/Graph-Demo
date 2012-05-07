/*
 * =====================================================================================
 *
 *       Filename:  graphwidget.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/31/2012 05:56:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "graphwidget.h"
#include "vertex.h"

//#include <QtGui>
#include <QKeyEvent>
#include <QWheelEvent>

#include <cmath>

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent)
{
//    QGraphicsScene *scene = new QGraphicsScene(this);
//    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
//    scene->setSceneRect(-300, -300, 600, 600);
//
//    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setTransformationAnchor(AnchorUnderMouse);

//    setMinimumSize(600, 600);
}

#if 0
void GraphWidget::itemMoved()
{

}
#endif

/* void GraphWidget::keyPressEvent(QKeyEvent *event)
 * {
 *     if(event->modifiers() == Qt::ControlModifier) {
 *         if(event->key() == Qt::Key_Plus
 *     }
 * 
 *     switch(event->key()) {
 *         case Qt::Key_Plus:
 *             zoomIn();
 *             break;
 *         case Qt::Key_Minus:
 *             zoomOut();
 *             break;
 *         case Qt::Key_Space:
 *         case Qt::Key_Enter:
 *             shuffle();
 *             break;
 *         default:
 *             QGraphicsView::keyPressEvent(event);
 *     }
 * }
 */

void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(std::pow(double(2), -event->delta() / 240.0));
}


void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if(factor < .07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void GraphWidget::resizeEvent(QResizeEvent *event) {
    event->accept();
    int w = std::max(event->size().width(), event->oldSize().width());
    int h = std::max(event->size().height(), event->oldSize().height());
    scene()->setSceneRect(-w/2,-h/2,w,h);
}

void GraphWidget::shuffle()
{
    int w = width() - 30, h = height() - 30;
    foreach (QGraphicsItem *item, scene()->items()) {
        if(qgraphicsitem_cast<Vertex *>(item))
            item->setPos(-w/2 + qrand() % w, -h/2+ qrand() % h);
    }
}

void GraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphWidget::zoomOut()
{
    scaleView(1/qreal(1.2));
}
