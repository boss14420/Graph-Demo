/*
 * =====================================================================================
 *
 *       Filename:  graphwidget.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/31/2012 05:50:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _GRAPHWIDGET_H_
#define _GRAPHWIDGET_H_

#include <QtGui/QGraphicsView>

class Vertex;

class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GraphWidget(QWidget *parent = 0);

//    void itemMoved();
public slots:
    void shuffle();
    void zoomIn();
    void zoomOut();

protected:
//    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event);

    void scaleView(qreal scaleFactor);
};

#endif
