/*
 * =====================================================================================
 *
 *       Filename:  vertex.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/31/2012 05:25:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _VERTEX_H_
#define _VERTEX_H_

#include <QGraphicsObject>
#include <QList>

class Edge;

class Vertex : public QGraphicsObject {
    Q_OBJECT
    Q_PROPERTY(VertexColor color READ color WRITE setColor)
    Q_PROPERTY(int order READ topoOrder WRITE setTopoOrder)
    Q_ENUMS(VertexColor)
public:
    Vertex(qreal x, qreal y, int id_ = numVertex++);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    enum { Type = UserType + 1 };
    int type() const { return Type; }

    int getId() const { return id; }

    void addInEdge(Edge *edge);
    void addOutEdge(Edge *edge);

    bool removeInEdge(Edge *edge);
    bool removeOutEdge(Edge *edge);

    bool hasPathTo(Vertex const*) const;

    QList<Edge *> inEdges() const;
    QList<Edge *> outEdges() const;

    enum VertexColor { InitColor = 0, VisitedColor, SourceColor, DestColor, DiscoveredColor};
    static QColor vertexColor[5][2];

    VertexColor color() const { return m_color; }
    void setColor(VertexColor);

    int topoOrder() const { return m_topoOrder; }
    void setTopoOrder(int);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
private:
    int id;
    static int numVertex;

    VertexColor m_color;
    int m_topoOrder;

    QList<Edge *> inEdgeList;
    QList<Edge *> outEdgeList;
};

#endif
