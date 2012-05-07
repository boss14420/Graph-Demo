/*
 * =====================================================================================
 *
 *       Filename:  dijkstra.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2012 09:08:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "dijkstra.h"
#include "mainwindow.h"
#include "edge.h"
#include "vertex.h"
#include "vertexlabel.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "dijkstra_visitor.hpp"

#include <QAbstractAnimation>
#include <QTableWidget>
#include <QMessageBox>

#include <QDebug>

DijkstraAnimation::DijkstraAnimation(MainWindow *mw) : Algorithm(mw) {}

void DijkstraAnimation::generateAnimList() {
    if(sourceVertex == nullptr) {
        QMessageBox::warning(mainWindow, tr("Dijkstra search Error!"),
                tr("You must specify the source vertex"));
        return;
    }

    /* Check for negative edge */
    bool hasNegativeEdge = false;
    for(Vertex *v : vertexList) {
        for(Edge *e : v->outEdges()) {
            if(e->property().weight < 0) {
                e->setZValue(1000.0);
                e->setState(Edge::Error);
                hasNegativeEdge = true;
            }
        }
    }
    if(hasNegativeEdge) {
        QMessageBox::warning(mainWindow, tr("Dijkstra search Error"), tr("This graph has negative edge"));
        return;
    }
    /* */


    typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_descriptor;
    typedef boost::graph_traits<graph_t>::edge_descriptor edge_descriptor;
    typedef boost::edge_bundle_type<graph_t>::type edge_property;

    graph_t g = getGraph<graph_t>();
    vertex_descriptor s = boost::vertex(vertexList.indexOf(sourceVertex), g);

    QStringList horizHeaderLabels;
    for(auto v : vertexList) horizHeaderLabels << QString::number(v->getId());
    QStringList vertHeaderLabels;
    vertHeaderLabels << tr("Distance") << tr("Predecessor");

    labelTable->setColumnCount(vertexList.size());
    labelTable->setRowCount(2);
    labelTable->setHorizontalHeaderLabels(horizHeaderLabels);
    labelTable->setVerticalHeaderLabels(vertHeaderLabels);

    labelVector.resize(vertexList.size());
    for(int i = 0; i < vertexList.size(); ++i) {
//        labelTable->setItem(0, i, new VertexLabel(0));
        labelVector[i] = 
            std::shared_ptr<VertexLabel>(new VertexLabel(labelTable, i));
    }

//    QSequentialAnimationGroup *animGroup = new QSequentialAnimationGroup(mw);

//    QVector<vertex_descriptor> predecssor_vertex(boost::num_vertices(g), -1);
    QVector<qint32> distance_map(vertexList.size(), 0);
//    QVector<vertex_descriptor> pred_map(vertexList.size(), -1);
    QVector<qint32> color_map(vertexList.size());

    vertex_descriptor d = -1;
    if(destVertex != nullptr) {
        d = boost::vertex(vertexList.indexOf(destVertex), g);
    }

    try {
        boost::dijkstra_shortest_paths(g, s,
                boost::weight_map(boost::get(&edge_property::weight, g))
                .distance_map(distance_map.begin())
//                .predecessor_map(pred_map.begin())
                .color_map(color_map.begin())
                .visitor(DijkstraVisitor<graph_t>(
                        s, d, vertexList, labelVector, distance_map, 
//                        pred_map, 
                        color_map,
                        animList
                        )
                    )
                );
    } catch (TargetFoundException& ex) {
    }
    
    for(auto& anim : animList) {
        connect(anim.get(), SIGNAL(finished()), this, SLOT(finishAnimation()));

#if 0
        // debug code
        auto *prlAnim = dynamic_cast<QParallelAnimationGroup*>(anim.get());
        qDebug() << prlAnim->metaObject()->className() << " : " << prlAnim->objectName();
        for(int i = 0; i < prlAnim->animationCount(); ++i) {
            qDebug() << '\t' << prlAnim->animationAt(i)->metaObject()->className();
        }
#endif
    }
}

Edge* DijkstraAnimation::newEdge(Vertex* source, Vertex* dest) {
    int weight = qrand() % 30;
    edge_tag::CustomWeight tag;
    return new Edge(source, dest, tag, weight);
}

Edge* DijkstraAnimation::newRandomEdge() {
    int size = vertexList.size();
    if(size < 2)
        return nullptr;
    if(nEdge == size*(size-1))
        return nullptr;

    // source
    unsigned sourceId;
    do {
        sourceId = qrand() % size;
    } while(vertexList[sourceId]->outEdges().size() == size - 1);
    Vertex *source = vertexList[sourceId];

    // dest
    unsigned destId;
    bool hasThisEdge = false;
    do {
        destId = qrand() % size;
        hasThisEdge = false;
        foreach(Edge *e, vertexList[sourceId]->outEdges()) {
            if(e->destVertex() == vertexList[destId]) {
                hasThisEdge = true;
                break;
            }
        }
    } while( (sourceId == destId) || hasThisEdge );
    Vertex *dest = vertexList[destId];

    return newEdge(source, dest);
}
