/*
 * =====================================================================================
 *
 *       Filename:  vertex.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/31/2012 05:34:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */


#include "vertex.h"
#include "edge.h"

#include <deque>
#include <unordered_set>

//#include <QtGui>
#include <QPainter>
#include <QRadialGradient>
#include <QStyle>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

int Vertex::numVertex = 0;

QColor Vertex::vertexColor[5][2] = { { QColor(Qt::darkYellow).light(120),
                                        QColor(Qt::yellow).light(120) },
                                { QColor(Qt::darkGreen).light(120),
                                        QColor(Qt::green).light(120) },
                                { QColor(Qt::darkRed).light(120),
                                        QColor(Qt::red).light(120) },
                                { QColor(Qt::darkMagenta).light(120),
                                        QColor(Qt::magenta).light(120) },
                                { QColor(Qt::darkGray).light(120),
                                        QColor(Qt::gray).light(120) }
                                };

Vertex::Vertex(qreal x, qreal y, int _id) : id(_id) {
    setPos(x,y);
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);

    m_color = InitColor;
    m_topoOrder = -1;
}

void Vertex::setColor(VertexColor vc) { 
    m_color = vc; 
    update(); 
}

void Vertex::setTopoOrder(int _order) {
    int old_order = m_topoOrder;
    m_topoOrder = _order;
    if( (_order < 0 && old_order>= 0)
            || (_order >= 0 && old_order< 0) )
        prepareGeometryChange();

    update();
}

QRectF Vertex::boundingRect() const {
    qreal adjust = 2;
    if(m_topoOrder < 0)
        return QRectF(-10 - adjust, -10 - adjust, 20 + adjust, 20 + adjust);
    else
        return QRectF(-10 - adjust, -23 - adjust, 20 + adjust, 33 + adjust);
}

void Vertex::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) {
//    painter->setPen(Qt::NoPen);
//    painter->setBrush(Qt::darkGray);
//    painter->drawEllipse(-7, -7, 20, 20);

    QRadialGradient gradient(-3, -3, 10);
    if(option->state & QStyle::State_Sunken) {
        gradient.setCenter(3,3);
        gradient.setFocalPoint(3,3);
        gradient.setColorAt(1, vertexColor[m_color][0]);
        gradient.setColorAt(0, vertexColor[m_color][1]);
    } else {
        gradient.setColorAt(0, vertexColor[m_color][1]);
        gradient.setColorAt(1, vertexColor[m_color][0]);
    }
    painter->setBrush(gradient);

    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-10, -10, 20, 20);

    painter->drawText(-7, -7, 14, 14, Qt::AlignCenter, QString::number(id), nullptr);

    if(m_topoOrder >= 0) {
        painter->setPen(QPen(Qt::black, 1));
        painter->drawText(-7, -23, 14, 14, Qt::AlignCenter, 
                QString("[%1]").arg(m_topoOrder), nullptr);
    }

    if(isSelected()) {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(vertexColor[m_color][0], 1, Qt::DashLine));
        painter->drawRect(-11, -11, 22, 22);
    }
}

void Vertex::addOutEdge(Edge *edge) {
    outEdgeList << edge;
    edge->adjust();
}

void Vertex::addInEdge(Edge *edge) {
    inEdgeList << edge;
    edge->adjust();
}

bool Vertex::removeOutEdge(Edge *edge) {
    return outEdgeList.removeOne(edge);
}

bool Vertex::removeInEdge(Edge *edge) {
    return inEdgeList.removeOne(edge);
}
QList<Edge *> Vertex::inEdges() const {
    return inEdgeList;
}

QList<Edge *> Vertex::outEdges() const {
    return outEdgeList;
}

bool Vertex::hasPathTo(Vertex const *target) const {
    std::unordered_set<Vertex const*> checked;
    std::deque<Vertex const*> queue { this };
    while(!queue.empty()) {
        Vertex const *v = queue.front();
        queue.pop_front();

        checked.insert(v);

        if(v == target)
            return true;

        for(auto e : v->outEdges()) {
            Vertex *adj = e->destVertex();

            if(checked.find(adj) == checked.end()) {
                queue.push_back(adj);
            }
        }
    }
    return false;
}

void Vertex::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    update();
    QGraphicsItem::mousePressEvent(event);
}

void Vertex::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    update();
    QGraphicsItem::mouseMoveEvent(event);
}

void Vertex::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

QVariant Vertex::itemChange(GraphicsItemChange change, const QVariant &value) {
    switch(change) {
    case ItemPositionHasChanged:
        foreach(Edge *edge, inEdgeList) {
            edge->adjust();
        }

        foreach(Edge *edge, outEdgeList) {
            edge->adjust();
        }
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}
