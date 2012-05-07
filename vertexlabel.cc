/*
 * =====================================================================================
 *
 *       Filename:  vertexlabel.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2012 09:11:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */


#include "vertexlabel.h"

#include <QTableWidget>
#include <QTableWidgetItem>

#include <limits>

VertexLabel::VertexLabel(QTableWidget *tableWidget, unsigned c, 
        int d /* = 0 */, int p /* = 0 */, bool hasRank /* = false */)
            : distance(d), predecessor(p)
{
    distanceItem = new QTableWidgetItem();
    setDist(d);

    tableWidget->setItem(0, c, distanceItem);

    predItem = new QTableWidgetItem();
    setPred(p);

    tableWidget->setItem(1, c, predItem);

    if(hasRank) {
        rankItem = new QTableWidgetItem();
        setRank(0);

        tableWidget->setItem(2, c, rankItem);
    } else
        rankItem = nullptr;
}

int VertexLabel::dist() const {
    return distance;
}

void VertexLabel::setDist(int d) {
    distance = d;
    distanceItem->setText( (d !=  std::numeric_limits<int>::max()) ?
            QString::number(distance) : "Infinity" );
}

int VertexLabel::pred() const {
    return predecessor;
}

void VertexLabel::setPred(int p) {
    predecessor = p;
    predItem->setText( (p != -1) ?
            QString::number(predecessor) : "-" );
}

int VertexLabel::rank() const {
    return estimate_distance;
}

void VertexLabel::setRank(int r) {
    estimate_distance = r;
    rankItem->setText( (r !=  std::numeric_limits<int>::max()) ?
            QString::number(estimate_distance) : "Infinity" );
}

int VertexLabel::isSelected() const {
    return ( (distanceItem->isSelected() != 0) ? 1 : 0 )
        +  ( (predItem->isSelected() != 0) ? 1 : 0)
        +  ( (rankItem != nullptr) ? ((rankItem->isSelected() != 0) ? 1 : 0) : 0);
}

void VertexLabel::setSelected(int s) {
    distanceItem->setSelected(s != 0);
    predItem->setSelected(s != 0);
    if(rankItem != nullptr)
        rankItem->setSelected(s != 0);
}
