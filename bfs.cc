/*
 * =====================================================================================
 *
 *       Filename:  bfs.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/02/2012 09:08:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "bfs.h"
#include "mainwindow.h"
#include "edge.h"
#include "vertex.h"
#include "vertexlabel.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/graph_utility.hpp>

#include "bfs_visitor.hpp"
//#include "bfs_singlepair_visitor.hpp"

#include <QAbstractAnimation>
#include <QTableWidget>
#include <QMessageBox>

#include <QDebug>

BFSAnimation::BFSAnimation(MainWindow *mw) : DijkstraAnimation(mw) {}

void BFSAnimation::generateAnimList() {
    if(nullptr == sourceVertex) {
        QMessageBox::warning(mainWindow, tr("BFS Error!"),
                tr("You must specify the source vertex"));
        return;
    }

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

    vertex_descriptor d = -1;
    if(destVertex != nullptr) {
        d = boost::vertex(vertexList.indexOf(destVertex), g);
    }
//        try {
//            boost::dijkstra_shortest_paths(g, s,
//                boost::weight_map(boost::get(&edge_property::weight, g))
//                    .visitor(BFSSinglePairVisitor<graph_t>(s, d, 
//                        vertexList, predecssor_vertex, animList)));
//        } catch( TargetFoundException& ex) {}
//    } else {
//
    try {
        boost::breadth_first_search(g, s,
            boost::visitor(BFSVisitor<graph_t>(s, d, vertexList, labelVector, animList)));
    } catch (TargetFoundException& ex) {}

//        boost::dijkstra_shortest_paths(g, s,
//            boost::weight_map(boost::get(&edge_property::weight, g))
//                .distance_map(distance_map.begin())
//                .visitor(BFSVisitor<graph_t>(
//                        s, vertexList, labelVector, distance_map, animList
//                                                )
//                        )
//                );
//    }
    
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

Edge* BFSAnimation::newEdge(Vertex* source, Vertex* dest) {
    edge_tag::NoWeight tag;
    return new Edge(source, dest, tag);
}

