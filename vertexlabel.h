/*
 * =====================================================================================
 *
 *       Filename:  vertexlabel.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2012 08:58:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _VERTEXLABEL_H_
#define _VERTEXLABEL_H_

#include <QObject>

class QTableWidget;
class QTableWidgetItem;

class VertexLabel : public QObject {
    Q_OBJECT
    Q_PROPERTY(int distance READ dist WRITE setDist)
    Q_PROPERTY(int predecessor READ pred WRITE setPred)
    Q_PROPERTY(int rank READ rank WRITE setRank)
    Q_PROPERTY(int selected READ isSelected WRITE setSelected)

public:
    VertexLabel(QTableWidget* tableWidget, unsigned r, 
            int d = 0, int p = 0, bool hasRank = false);

    int dist() const;
    void setDist(int);

    int pred() const;
    void setPred(int);

    int rank() const;
    void setRank(int);

    int isSelected() const;
    void setSelected(int);

private:
//    unsigned row;
    int distance, predecessor, estimate_distance;
    QTableWidgetItem *distanceItem, *predItem, *rankItem;
};

#endif
