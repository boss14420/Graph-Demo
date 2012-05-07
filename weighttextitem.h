/*
 * =====================================================================================
 *
 *       Filename:  weighttextitem.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/05/2012 03:28:38 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _WEIGHTTEXTITEM_H_
#define _WEIGHTTEXTITEM_H_

#include <QGraphicsTextItem>

class WeightTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    WeightTextItem(QString const& text, QGraphicsItem *parent = 0);

    enum { Type = UserType + 3 };
    int type() const { return Type; }

signals:
    void lostFocus(int);
    void selectedChanged(QGraphicsItem*);

protected:
//    QRectF boundingRect() const;
//    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

//    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void focusOutEvent(QFocusEvent*);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*);
};

#endif
