/*
 * =====================================================================================
 *
 *       Filename:  graphscene.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/04/2012 10:14:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <QtGui>
#include <QDebug>

#include "edge.h"
//#include "weighttextitem.h"
#include "vertex.h"
#include "graphscene.h"
#include "algorithm.h"

GraphScene::GraphScene(QMenu *menu,  Algorithm* algo, QObject *parent) 
    : QGraphicsScene(parent), contextMenu(menu), algorithm(algo)
{
    mode = MoveItem;
    line = 0;
//    nEdge = 0;
//    sourceVertex = destVertex = nullptr;

    connect(this, SIGNAL(graphChanged()), this, SLOT(setModified()));
}

void GraphScene::setMode(Mode _mode) {
    mode = _mode;
}

void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if(mouseEvent->button() != Qt::LeftButton)
        return;

    QPointF pos = mouseEvent->scenePos();

    Vertex *vertex;
    switch(mode) {
        case InsertVertex:
            vertex = new Vertex(pos.x(), pos.y());
            addItem(vertex);
//            vertexList << vertex;
            algorithm->addVertex(vertex);
            emit itemInserted(vertex);
            emit graphChanged();
            break;
        case InsertEdge:
            line = new QGraphicsLineItem(QLineF(pos, pos));
            line->setPen(QPen(Qt::black, 2));
            addItem(line);
            emit graphChanged();
        default:
            ;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if(mode == InsertEdge && line != 0) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else if(mode == MoveItem) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

void GraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if(line != 0 && mode == InsertEdge) {
        Vertex *sourceVertex = firstVertex(line->line().p1())
                , *destVertex = firstVertex(line->line().p2());

        removeItem(line);
        delete line;

        if(sourceVertex && destVertex && (sourceVertex != destVertex)) 
        {
            bool vertexsConnected = false;
            foreach(Edge *edge, sourceVertex->outEdges()) {
                if(edge->destVertex() == destVertex) {
                    vertexsConnected = true;
                    break;
                }
            }
            if(!vertexsConnected) {
//                Edge *edge = new Edge(sourceVertex, destVertex, 10);
                Edge *edge = algorithm->newEdge(sourceVertex, destVertex);

                if(edge != nullptr) {
                    edge->setZValue(-1000.0);
                    addItem(edge);
                    edge->adjust();
                    connect(edge, SIGNAL(edgeWeightChanged()), 
                            this, SIGNAL(graphChanged()));

                    algorithm->incEdge();
                    //                ++nEdge;
                    //                algorithm->incEdge();

                    emit graphChanged();
                }
            }
        }
    }

    line = 0;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void GraphScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    if(mode == MoveItem) {
        Vertex *selectedVertex = firstVertex(event->scenePos());
//        algorithm->setSelectedVertex(selectedVertex);
        algorithm->selectedVertex = selectedVertex;
        if(selectedVertex != nullptr) {
            clearSelection();
            selectedVertex->setSelected(true);

            contextMenu->actions()[0]->setChecked(false);
            contextMenu->actions()[1]->setChecked(false);
            if(selectedVertex == algorithm->sourceVertex) 
                contextMenu->actions()[0]->setChecked(true);
            else if(selectedVertex == algorithm->destVertex)
                contextMenu->actions()[1]->setChecked(true);
            contextMenu->exec(event->screenPos());
        }
    }
}
/* bool GraphScene::isItemChange(int type) {
 *     return false;
 * }
 */

void GraphScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
        painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
        painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersect(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

//#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5)
#if 0
    // Text
    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
            sceneRect.width() - 4, sceneRect.height() - 4);
    QString message(tr("Click and drag the nodes around, and zoom with the mouse "
                "wheel or the '+' and '-' keys"));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    painter->drawText(textRect.translated(2, 2), message);
    painter->setPen(Qt::black);
    painter->drawText(textRect, message);
#endif
}
Vertex* GraphScene::firstVertex(QPointF pos) {
    QList<QGraphicsItem*> selectedItems = items(pos);
    QList<QGraphicsItem*>::iterator li;
    for(li = selectedItems.begin(); li < selectedItems.end(); ++li)
        if((*li)->type() == Vertex::Type)
            return qgraphicsitem_cast<Vertex*>(*li);
    return nullptr;
}

void GraphScene::addRandomVertex() {
    int w = width() - 30, h = height() -30;
    Vertex *v = new Vertex(-w/2 + qrand() % w, -h/2 + qrand() % h);
//    vertexList << v;
    algorithm->addVertex(v);
    addItem(v);

    emit graphChanged();
}

void GraphScene::addRandomEdge() {

    Edge *edge = algorithm->newRandomEdge();
    if(edge != nullptr) {
        addItem(edge);
        algorithm->incEdge();
        connect(edge, SIGNAL(edgeWeightChanged()),
                this, SIGNAL(graphChanged()));
        emit graphChanged();
    }

//    int size = vertexList.size();
//    if(size < 2)
//        return;
////    if(edgeList.size() == size*(size-1))
////        return;
//    if(nEdge == size*(size - 1))
//        return;
//
//    // source
//    unsigned sourceId;
//    do {
//        sourceId = qrand() % size;
//    } while(vertexList[sourceId]->outEdges().size() == size - 1);
//    Vertex *source = vertexList[sourceId];
//    
//    // dest
//    unsigned destId;
//    bool hasThisEdge = false;
//    do {
//        destId = qrand() % size;
//        hasThisEdge = false;
//        foreach(Edge *e, vertexList[sourceId]->outEdges()) {
//            if(e->destVertex() == vertexList[destId]) {
//                hasThisEdge = true;
//                break;
//            }
//        }
//    } while((sourceId == destId) || hasThisEdge);
//    Vertex *dest = vertexList[destId];
//
//    // weight
//    unsigned weight = qrand() % 30;
//    Edge *edge = new Edge(source, dest, weight);
//    addItem(edge);
//    connect(edge, SIGNAL(edgeWeightChanged()),
//            this, SIGNAL(graphChanged()));
//    ++nEdge;

/*     WeightTextItem *wti = new WeightTextItem("10", edge);
 *     addItem(wti);
 *     connect(wti, SIGNAL(lostFocus(unsigned, WeightTextItem*)), 
 *             this, SLOT(changeEdgeWeight(unsigned, WeightTextItem*)));
 *     setFocusItem(wti);
 */

//    edgeList << edge_t(vertexList[sourceId]->getId(), 
//                        vertexList[destId]->getId());
//    propertyList << (EdgeProperty<unsigned>) {weight};

//    emit graphChanged();
}


#if 0
void GraphScene::dijkstraFindPath() {
    unsigned size = vertexList.size();

    if(size < 2)
        return;

    if(nullptr == sourceVertex)
        return;

    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
                boost::no_property, EdgeProperty<unsigned> > graph_t;
    typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_descriptor;
    typedef boost::graph_traits<graph_t>::edge_descriptor edge_descriptor;

    QList<edge_t> edgeList;
    QList<EdgeProperty<unsigned> >propertyList;

    QHash<Vertex*, unsigned> idMap;
    for(unsigned i = 0; i < size; ++i)
        idMap[vertexList[i]] = i;

    for(unsigned i = 0; i < size; ++i) {
        foreach(Edge *e, vertexList[i]->outEdges()) {
            unsigned j = idMap[e->destVertex()];
            edgeList << edge_t(i, j);
            propertyList << e->property();
        }
    }

    graph_t g(edgeList.begin(), edgeList.end(), propertyList.begin(), vertexList.size());
    vertex_descriptor s = boost::vertex(vertexList.indexOf(sourceVertex), g);
    QSequentialAnimationGroup *animGroup = new QSequentialAnimationGroup(this);

    if(destVertex != nullptr) {
        vertex_descriptor d = boost::vertex(vertexList.indexOf(destVertex), g);
        QVector<vertex_descriptor> predecssor_vertex(vertexList.size(), -1);
        try {
            boost::dijkstra_shortest_paths(g, s,
                    boost::weight_map(boost::get(&EdgeProperty<unsigned>::weight, g))
                        .visitor(DijkstraSinglePairVisitor<graph_t>(s, d, vertexList, predecssor_vertex, animGroup)));
        } catch(TargetFoundException& ex) { }
    } else {
        boost::dijkstra_shortest_paths(g, s, 
            boost::weight_map(boost::get(&EdgeProperty<unsigned>::weight, g))
                .visitor(DijkstraVisitor<graph_t>(s, vertexList, animGroup)));
    }
    animGroup->start();
    emit graphChanged();
}
#endif

void GraphScene::removeSelectedItems() {
    QList<QGraphicsItem *> toRemoveItems;

    foreach(QGraphicsItem *item, selectedItems()) {
        if(item->type() == Vertex::Type) {
            Vertex *vertex = qgraphicsitem_cast<Vertex*>(item);
            foreach(Edge* e, vertex->outEdges()) {
                if(!toRemoveItems.contains(e)) {
                    e->destVertex()->removeInEdge(e);
                    toRemoveItems << e;
//                    --nEdge;
                    algorithm->decEdge();
                }
            }
            foreach(Edge* e, vertex->inEdges()) {
                if(!toRemoveItems.contains(e)) {
                    e->sourceVertex()->removeOutEdge(e);
                    toRemoveItems << e;
//                    --nEdge;
                    algorithm->decEdge();
                }
            }
//            vertexList.removeOne(vertex);
            algorithm->removeVertex(vertex);
            toRemoveItems << item;
        } else if(item->type() == Edge::Type) {
            if(!toRemoveItems.contains(item)) {
                Edge *e = qgraphicsitem_cast<Edge*>(item);
                e->sourceVertex()->removeOutEdge(e);
                e->destVertex()->removeInEdge(e);
                toRemoveItems << e;
//                --nEdge;
                algorithm->decEdge();
            }
        }
    }

    if(!toRemoveItems.empty()) {
        foreach(QGraphicsItem *item, toRemoveItems) {
            if(item->type() == Vertex::Type) {
                Vertex *v = qgraphicsitem_cast<Vertex*>(item);
                if(algorithm->sourceVertex == v)
                    algorithm->sourceVertex = nullptr;
                else if(algorithm->destVertex == v)
                    algorithm->destVertex = nullptr;
                else if(algorithm->selectedVertex == v)
//                    algorithm->setSelectedVertex(nullptr);
                    algorithm->selectedVertex = nullptr;
            } 
            removeItem(item);
        }
        emit graphChanged();
    }
}

void GraphScene::clearAllItems() {
    clear();
//    edgeList.clear();
//    propertyList.clear();
//    nEdge = 0;
    
    algorithm->vertexList.clear();
    algorithm->selectedVertex = algorithm->sourceVertex
        = algorithm->destVertex = nullptr;

    algorithm->clearEdge();
    emit graphChanged();
}

void GraphScene::setSource() {
    algorithm->setSource();
}

void GraphScene::setDest() {
    algorithm->setDest();
}

void GraphScene::loadFile(std::ifstream &in) {
}

void GraphScene::saveToFile(std::ofstream &out) {
}
