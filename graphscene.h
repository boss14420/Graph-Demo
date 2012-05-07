/*
 * =====================================================================================
 *
 *       Filename:  graphscene.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/04/2012 10:06:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _GRAPHSCENE_H_
#define _GRAPHSCENE_H_

#include <QGraphicsScene>
#include <QList>

#include <fstream>

class Edge;
class Vertex;
class Algorithm;

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QGraphicsSceneContextMenuEvent;
class QPointF;
class QGraphicsLineItem;
class QColor;
class QAction;
class QMenu;
QT_END_NAMESPACE

class GraphScene : public QGraphicsScene {
    Q_OBJECT
    Q_PROPERTY(bool modified READ isModified WRITE setModified)

public:
    GraphScene(QMenu *menu, Algorithm* algo, QObject *parent = 0);
    
    enum Mode { InsertVertex, InsertEdge, MoveItem };
    void setMode(Mode);

    void loadFile(std::ifstream&);
    void saveToFile(std::ofstream&);

    bool isModified() const { return m_modified; }

public slots:
    void addRandomVertex();
    void addRandomEdge();
//    void dijkstraFindPath();

    void removeSelectedItems();
    void clearAllItems();

    void setSource();
    void setDest();

    void setModified(bool m = true) { m_modified = m; }

//    template<typename graph_t> graph_t getGraph();
signals:
    void itemInserted(QGraphicsItem *);
    void itemSelected(QGraphicsItem *);
    void graphChanged();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
    
    void drawBackground(QPainter *, const QRectF&);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *);
private:
    bool isItemChange(int type);
    Vertex *firstVertex(QPointF);

    int nEdge;
    bool m_modified;

    Mode mode;
    bool leftButtonDown;
    QPointF startPoint;
    QGraphicsLineItem *line;

//    QList<Vertex*> vertexList;

//    typedef std::pair<unsigned, unsigned> edge_t;
//    QList<edge_t> edgeList;
//    QList<EdgeProperty<unsigned> > propertyList;

    QMenu *contextMenu;
//    Vertex *sourceVertex;
//    Vertex *destVertex;
//    Vertex *selectedVertex;

    Algorithm *algorithm;
};

#endif
