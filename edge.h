/*
 * =====================================================================================
 *
 *       Filename:  edge.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/03/2012 04:16:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _EDGE_H_
#define _EDGE_H_

#include <QGraphicsLineItem>

class Vertex;
class WeightTextItem;

/* Edge tag */
namespace edge_tag {
    struct CustomWeight {};
    struct WeightByPos {};
    struct NoWeight {};
}
/* */

template <typename T>
    struct EdgeProperty { T weight; };

class Edge : public QObject, public QGraphicsLineItem
{

    Q_OBJECT
    Q_PROPERTY(EdgeState state READ state WRITE setState)
    Q_ENUMS(EdgeState)

    struct State {
        QColor color;
        int linewidth;
    };
public:
    Edge(Vertex*, Vertex*, edge_tag::NoWeight, QGraphicsItem* parent = 0);
    Edge(Vertex*, Vertex*, edge_tag::CustomWeight, int, QGraphicsItem* parent = 0);
    Edge(Vertex*, Vertex*, edge_tag::WeightByPos, QGraphicsItem* parent = 0);

    int calculateDistance() const;

    Vertex *sourceVertex() const;
    Vertex *destVertex() const;

    void adjust();

    enum { Type = UserType + 2 };
    int type() const { return Type; }

    enum EdgeState {Init = 0, Relaxed, InPath, Error};
    static State edgeState[4];

    EdgeState state() const { return m_state; }
    void setState(EdgeState ec) { m_state = ec; update();}

    EdgeProperty<int> property();
signals:
    void edgeWeightChanged();

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override;

private:
    Vertex *source, *dest;

    QPointF sourcePoint;
    QPointF destPoint;

    EdgeState m_state;

    EdgeProperty<int> m_property;

    WeightTextItem *weightTextItem;

    bool weightbypos;

    static qreal arrowSize;
    static int delayTime;
private slots:
    void weightTextItemLostFocus(int);
};

#endif
