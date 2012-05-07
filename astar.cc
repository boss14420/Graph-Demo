/*
 * =====================================================================================
 *
 *       Filename:  astar.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/03/2012 09:08:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "astar.h"
#include "mainwindow.h"
#include "edge.h"
#include "vertex.h"
#include "vertexlabel.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
//#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "astar_visitor.hpp"

#include <QAbstractAnimation>
#include <QTableWidget>
#include <QMessageBox>

#include <QDebug>

AStarAnimation::AStarAnimation(MainWindow *mw) : DijkstraAnimation(mw) {}

void AStarAnimation::generateAnimList() {
    if(sourceVertex == nullptr) {
        QMessageBox::warning(mainWindow, tr("A* search Error!"),
                tr("You must specify the source vertex"));
        return;
    } else if(nullptr == destVertex) {
        QMessageBox::warning(mainWindow, tr("A* search Error!"),
                tr("You must specify the target vertex"));
        return;
    }

    typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_descriptor;
    typedef boost::graph_traits<graph_t>::edge_descriptor edge_descriptor;
    typedef boost::edge_bundle_type<graph_t>::type edge_property;

    graph_t g = getGraph<graph_t>();
    vertex_descriptor s = boost::vertex(vertexList.indexOf(sourceVertex), g)
                    , d = boost::vertex(vertexList.indexOf(destVertex), g);

    QStringList horizHeaderLabels;
    for(auto v : vertexList) horizHeaderLabels << QString::number(v->getId());
    QStringList vertHeaderLabels;
    vertHeaderLabels << tr("Distance") << tr("Predecessor") << tr("Estimate");

    labelTable->setColumnCount(vertexList.size());
    labelTable->setRowCount(3);
    labelTable->setHorizontalHeaderLabels(horizHeaderLabels);
    labelTable->setVerticalHeaderLabels(vertHeaderLabels);

    labelVector.resize(vertexList.size());
    for(int i = 0; i < vertexList.size(); ++i) {
//        labelTable->setItem(0, i, new VertexLabel(0));
        labelVector[i] = 
            std::shared_ptr<VertexLabel>(new VertexLabel(labelTable, i, 0, 0, true));
    }

//    QSequentialAnimationGroup *animGroup = new QSequentialAnimationGroup(mw);

//    QVector<vertex_descriptor> pred_map(boost::num_vertices(g), -1);

    QVector<qint32> distance_map(vertexList.size(), 0);
    QVector<qint32> color_map(vertexList.size());

//    QVector<qint32> rank_map(vertexList.size(), std::numeric_limits<qint32>::max());
//    rank_map[s] = 0;

    AStarHeuristic<graph_t, qint32> heuristic(vertexList, destVertex);
    try {
        boost::astar_search(g, s, heuristic,
                boost::weight_map(boost::get(&edge_property::weight, g))
                .distance_map(distance_map.begin())
//                .predecessor_map(pred_map.begin())
//                .rank_map(rank_map.begin())
                .color_map(color_map.begin())
                .visitor(AStarVisitor<graph_t, qint32>(
                            s, d, vertexList, labelVector, 
                            distance_map, /* pred_map, rank_map,*/
                            color_map,
                            heuristic,
                            animList
                        )
                    )
                );
    } catch (TargetFoundException& ex) {
        for(auto& anim : animList) {
            connect(anim.get(), SIGNAL(finished()), this, SLOT(finishAnimation()));
        }
        return;
    }
    
    QMessageBox::warning(mainWindow, tr("A* search error!"), tr("Target not found"));
    animList.clear();
}

Edge* AStarAnimation::newEdge(Vertex* source, Vertex* dest) {
//    int weight = qrand() % 30;
    edge_tag::WeightByPos tag;
    return new Edge(source, dest, tag);
}
