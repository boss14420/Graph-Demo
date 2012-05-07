/*
 * =====================================================================================
 *
 *       Filename:  weighttextitem.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/05/2012 03:31:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "weighttextitem.h"

#include <QDebug>

WeightTextItem::WeightTextItem(QString const& text, QGraphicsItem *parent)
    : QGraphicsTextItem(text, parent)
{
    setFlags(QGraphicsItem::ItemIsSelectable);
}

//QVariant WeightTextItem::itemChange(GraphicsItemChange change, const QVariant &value)
//{
//    if(change == QGraphicsItem::ItemSelectedHasChanged)
//        emit selectedChanged(this);
//    return value;
//}

void WeightTextItem::focusOutEvent(QFocusEvent *event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    int weight = toPlainText().toInt();
    setHtml(QString::number(weight));
    emit lostFocus(weight);
    QGraphicsItem::focusOutEvent(event);
}

void WeightTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(textInteractionFlags() == Qt::NoTextInteraction)
        setTextInteractionFlags(Qt::TextEditorInteraction);
    QGraphicsItem::mouseDoubleClickEvent(event);
}

/* void WeightTextItem::paint(QPainter * painter, const QStyleOptionGraphicsItem *style, QWidget *widget) {
 * 
 *     QGraphicsTextItem::paint(painter, style, widget);
 * 
 *     QPointF sourcePoint = edge->sourcePoint, destPoint = edge->destPoint;
 *     QPointF mid = (sourcePoint + destPoint) / 2;
 *     //    qreal adjust = (sourcePoint.y() < destPoint.y()) ? 10 : -10;
 *     qreal adjust = 14, adjustX, adjustY;
 *     if(destPoint.y() > sourcePoint.y()) adjustX = -adjust;
 *     else adjustX = adjust / 3; 
 * 
 *     if(destPoint.x() > sourcePoint.x()) adjustY = 0;
 *     else adjustY = -adjust;
 * 
 *     //    painter->drawText(QPointF(mid.x() + adjustX, mid.y() + adjustY) 
 *     //                        , QString::number(weight));
 * 
 *     setPos(QPointF(mid.x() + adjustX, mid.y() + adjustY));
 *     setDefaultTextColor(edge->color);
 *     setHtml(toPlainText());
 * }
 * 
 */
