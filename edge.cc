/*
 * =====================================================================================
 *
 *       Filename:  edge.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/03/2012 04:21:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <QPainter>
#include <QDebug>

#include "edge.h"
#include "vertex.h"
#include "weighttextitem.h"

#include <cmath>

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

qreal Edge::arrowSize = 10;
int Edge::delayTime = 2000;

Edge::State Edge::edgeState[4] = { { Qt::black, 1} , { Qt::black, 2},
                    {QColor(141, 56, 201), 2} , { Qt::red, 2 } };


/*
 *--------------------------------------------------------------------------------------
 *       Class:  Edge
 *      Method:  Edge :: Edge
 * Description:  Constructor for no-weight edge
 *--------------------------------------------------------------------------------------
 */
Edge::Edge(Vertex *_source, Vertex *_dest, edge_tag::NoWeight, 
                QGraphicsItem *parent /* = 0 */)
    : QGraphicsLineItem(parent), source(_source), dest(_dest),
        m_state(Init), weightTextItem(nullptr), weightbypos(false)
{
    setFlag(ItemIsSelectable);

    setPen(QPen(edgeState[m_state].color, edgeState[m_state].linewidth,
                Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    setAcceptedMouseButtons(Qt::LeftButton);
    source->addOutEdge(this);
    dest->addInEdge(this);

    m_property = { 1 };
    adjust();
}
/* */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  Edge
 *      Method:  Edge :: Edge
 * Description:  Constructor for custom weight edge
 *--------------------------------------------------------------------------------------
 */
Edge::Edge(Vertex *_source, Vertex *_dest, edge_tag::CustomWeight, 
        int weight, QGraphicsItem *parent /* = 0 */)
            : Edge(_source, _dest, edge_tag::NoWeight(), parent)
{
    m_property = { weight };
    weightTextItem = new WeightTextItem(QString::number(weight), this);
    connect(weightTextItem, SIGNAL(lostFocus(int)),
                this, SLOT(weightTextItemLostFocus(int)));

    adjust();
}
/* */


/*
 *--------------------------------------------------------------------------------------
 *       Class:  Edge
 *      Method:  Edge :: Edge
 * Description:  Constructor for weight-by-position edge
 *--------------------------------------------------------------------------------------
 */
Edge::Edge(Vertex *_source, Vertex *_dest, edge_tag::WeightByPos,
                QGraphicsItem *parent /*  = 0 */)
    : Edge(_source, _dest, edge_tag::NoWeight(), parent) 
{
    weightbypos = true;

    int weight = calculateDistance();
    m_property = { weight };
    weightTextItem = new WeightTextItem(QString::number(weight), this);
    weightTextItem->setAcceptedMouseButtons(Qt::NoButton);

    adjust();
}
/* */


int Edge::calculateDistance() const {
    QPointF p1 = source->mapToScene(0,0),
            p2 = dest->mapToScene(0,0);

    int dx = p2.x() - p1.x();
    int dy = p2.y() - p1.y();

    return static_cast<int>(std::sqrt(dx*dx+dy*dy)+.5);
}

Vertex *Edge::destVertex() const {
    return dest;
}

Vertex *Edge::sourceVertex() const {
    return source;
}

EdgeProperty<int> Edge::property() const {
    return m_property; 
}

void Edge::setProperty(EdgeProperty<int> p) {
    m_property = p;
    update();
}

void Edge::adjust() {
    if(!source || !dest)
        return;

    QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
    
    qreal length = line.length();

    prepareGeometryChange();

    if(length > qreal(20.)) {
        QPointF edgeOffset((line.dx() * 10)/ length, (line.dy() * 10)/ length);
        sourcePoint = line.p1() + edgeOffset;
        destPoint = line.p2() - edgeOffset;
    } else {
        sourcePoint = destPoint = line.p2();
    }

    setLine({sourcePoint, destPoint});

    if(weightTextItem != nullptr) {
        // re calculating weight
        if(weightbypos) {
            m_property.weight = calculateDistance();
            weightTextItem->setPlainText(QString::number(m_property.weight));
            emit edgeWeightChanged();
        }

        // set weightTextItem pos
        QPointF mid = (sourcePoint + destPoint * 2) / 3;
        //    qreal adjust = (sourcePoint.y() < destPoint.y()) ? 10 : -10;
        qreal adjust = 14, adjustX, adjustY;
        if(destPoint.y() > sourcePoint.y()) adjustX = -adjust;
        else adjustX = adjust / 3; 

        if(destPoint.x() > sourcePoint.x()) adjustY = 0;
        else adjustY = -adjust;

        weightTextItem->setPos(QPointF(mid.x() + adjustX, mid.y() + adjustY));
    }

}

QRectF Edge::boundingRect() const {
    if(!source || !dest)
        return QRectF();

    qreal penWidth = 1;
    qreal extra = (penWidth + arrowSize) / 2.0 + (weightTextItem ? 14 : 0);

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
                    .normalized()
                    .adjusted(-extra, -extra, extra, extra);
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(!source || !dest)
        return;

    if(source->collidesWithItem(dest))
        return;
//
//    QLineF line(sourcePoint, destPoint);
//    if(qFuzzyCompare(line.length(), qreal(0.)))
//        return;

    // Draw the line itself
    painter->setPen(QPen(edgeState[m_state].color, edgeState[m_state].linewidth, 
                Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line());

    // Draw the arrow
    double angle = std::acos(line().dx() / line().length());
    if(line().dy() >= 0)
        angle = TwoPi - angle;
    
    QPointF arrowPoint = (sourcePoint + destPoint*2)/3;

    QPointF ArrowP1 = arrowPoint + QPointF(std::sin(angle - Pi/3) * arrowSize,
                                          std::cos(angle - Pi/3) * arrowSize);
    QPointF ArrowP2 = arrowPoint + QPointF(std::sin(angle - Pi + Pi/3) * arrowSize,
                                          std::cos(angle - Pi + Pi/3) * arrowSize);

    painter->setBrush(edgeState[m_state].color);
    painter->drawPolygon(QPolygonF() << arrowPoint << ArrowP1 << ArrowP2);

    if(weightTextItem != nullptr) {
        // Draw weight number
        weightTextItem->setDefaultTextColor(edgeState[m_state].color);
        //    weightTextItem->setPlainText(weightTextItem->toPlainText());
    }
    
    // Draw border
    if(isSelected()) {
        painter->setPen(QPen(edgeState[m_state].color, 
                    edgeState[m_state].linewidth, Qt::DashLine));
        QLineF myLine = line();
        myLine.translate(0, 4.0);
        painter->drawLine(myLine);
        myLine.translate(0,-8.0);
        painter->drawLine(myLine);
    }
}

void Edge::weightTextItemLostFocus(int weight) {
    m_property.weight = weight;
    emit edgeWeightChanged();
}
