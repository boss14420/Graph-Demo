/*
 * =====================================================================================
 *
 *       Filename:  bellman_ford.cc
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

#include "bellman_ford.h"

#include "mainwindow.h"
#include "edge.h"
#include "vertex.h"
#include "vertexlabel.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>

#include "bellman_ford_visitor.hpp"

#include <QAbstractAnimation>
#include <QTableWidget>
#include <QMessageBox>

#include <QDebug>

BellmanFordAnimation::BellmanFordAnimation(MainWindow *mw) 
    : DijkstraAnimation(mw) 
{
        
}

void BellmanFordAnimation::generateAnimList() {
    if(nullptr == sourceVertex) {
        QMessageBox::warning(mainWindow, tr("Bellman-Ford search Error!"),
                tr("You must specify the source vertex"));
        return;
    }

    typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_descriptor;
    typedef boost::graph_traits<graph_t>::edge_descriptor edge_descriptor;
    typedef boost::edge_bundle_type<graph_t>::type edge_property;

    initAllItems();

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
            std::shared_ptr<VertexLabel>(
                new VertexLabel(labelTable, i, std::numeric_limits<int>::max(), -1)
                                        );
    }
    labelVector[s]->setDist(0);

//    QSequentialAnimationGroup *animGroup = new QSequentialAnimationGroup(mw);

//    QVector<vertex_descriptor> predecssor_vertex(boost::num_vertices(g), -1);
    QVector<qint32> distance_map(vertexList.size(),  std::numeric_limits<int>::max());
    distance_map[s] = 0;

    QVector<vertex_descriptor> predecessor_map(vertexList.size());

    bool success = boost::bellman_ford_shortest_paths(g, vertexList.size(),
            boost::weight_map(boost::get(&edge_property::weight, g))
            .predecessor_map(predecessor_map.begin())
            .distance_map(distance_map.begin())
            .visitor(BellmanFordVisitor<graph_t>(
                    s, vertexList, labelVector, distance_map, 
                        /* predecessor_map,*/ animList
                    )
                )
            );
//    }
    if(!success) {
        animList.clear();
        QMessageBox::warning(mainWindow, tr("Bellman-Ford search Error"), tr("This graph has negative cycle"));
        return;
    }

    /* get result */
    for(vertex_descriptor v = 0; v != boost::num_vertices(g); ++v) {
        if(v != s) {
            auto prlAnim =
                std::shared_ptr<QParallelAnimationGroup>(new QParallelAnimationGroup); 
            prlAnim->addAnimation(create(vertexList[v], "color",
                        Vertex::DiscoveredColor, Vertex::VisitedColor));

            vertex_descriptor pv = predecessor_map[v];
            for(Edge *edge : vertexList[pv]->outEdges()) {
                if(edge->destVertex() == vertexList[v]) {
                    prlAnim->addAnimation(create(edge, "state", Edge::Relaxed, Edge::InPath));
                    break;
                }
            }
            animList << prlAnim;
        }
    }

    for(auto& anim : animList) {
        connect(anim.get(), SIGNAL(finished()), this, SLOT(finishAnimation()));
    }
}

Edge* BellmanFordAnimation::newEdge(Vertex* source, Vertex* dest) {
    int weight = qrand() % 30 - 12;
    edge_tag::CustomWeight tag;
    return new Edge(source, dest, tag, weight);
}
