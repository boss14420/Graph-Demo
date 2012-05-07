/*
 * =====================================================================================
 *
 *       Filename:  dag.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/06/2012 11:44:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "dag.h"
#include "vertexlabel.h"
#include "vertex.h"
#include "edge.h"
#include "mainwindow.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dag_shortest_paths.hpp>

#include "dag_visitor.hpp"

#include <QAbstractAnimation>
#include <QTableWidget>
#include <QMessageBox>

DAGAnimation::DAGAnimation(MainWindow *mw) : Algorithm(mw) {}

void DAGAnimation::generateAnimList() {
    if(sourceVertex == nullptr) {
        QMessageBox::warning(mainWindow, tr("Error!"),
                tr("You must specify the source vertex"));
        return;
    }
        
    typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_descriptor;
    typedef boost::graph_traits<graph_t>::edge_descriptor edge_descriptor;
    typedef boost::edge_bundle_type<graph_t>::type edge_property;

    graph_t g = getGraph<graph_t>();
    vertex_descriptor s = boost::vertex(vertexList.indexOf(sourceVertex), g),
                      d = -1;
    if(destVertex != nullptr)
        d = boost::vertex(vertexList.indexOf(destVertex), g);


    /* topological sort */

    QList<vertex_descriptor> rev_topo_order;
    QVector<qint32> color_map(vertexList.size());
    try {
        boost::depth_first_visit(g, s,
                DFSVisitor<graph_t>(s, d, vertexList, rev_topo_order, animList),
                color_map.begin());
    } catch(CycleFoundException&) {
        animList.clear();
        return;
    }

    // indexing
    QHash<Vertex*, qint32> topoOrder;
    for(Vertex *v : vertexList) topoOrder[v] = -1;

    int count = rev_topo_order.size();
    for(int i = 0; i < count; ++i) {
        Vertex *vertex = vertexList[rev_topo_order[i]];
        auto prlAnim = std::shared_ptr<QParallelAnimationGroup>
                        (new QParallelAnimationGroup());

        prlAnim->addAnimation(create(vertex, "order", -1, count - i - 1));
        topoOrder[vertex] = count - i -1;

        if(vertex != sourceVertex && vertex != destVertex)
            prlAnim->addAnimation(create(vertex, "color",
                        Vertex::DiscoveredColor, Vertex::InitColor));

        animList << prlAnim;
    }

    /* end topological sort */


    /* search for shortest path */

    QStringList horizHeaderLabels;
    for(auto v : vertexList) 
        horizHeaderLabels << QString("%1 (%2)").arg(v->getId()).arg(topoOrder[v]);
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

    QVector<qint32> distance_map(vertexList.size(),0);
//    color_map.assign(vertexList.size(), boost::color_traits<qint32>::white());

    try {
        boost::dag_shortest_paths(g, s,
                boost::weight_map(boost::get(&edge_property::weight, g))
                .distance_map(distance_map.begin())
//                .color_map(color_map.begin())
                .visitor(DAGVisitor<graph_t>(
                        s, d, vertexList, labelVector, distance_map,
//                        color_map,
                        animList
                        )
                    )
                );
    } catch(TargetFoundException& ex) {

    }

    /* end search */

    for(auto& anim : animList)
        connect(anim.get(), SIGNAL(finished()), this, SLOT(finishAnimation()));

}

Edge* DAGAnimation::newEdge(Vertex *source, Vertex *dest) {
#ifdef USE_CHILD_AND_PARENT_SET
    // connect if not has a path from dest -> source
    if(!childSet[dest].contains(source)) {
        childSet[source] << dest;
        childSet[source] |= childSet[dest]; 

        for(Vertex *v : parentSet[source]) {
            childSet[v] << dest;
            childSet[v] |= childSet[dest];
        }

        parentSet[dest] << source;
        parentSet[dest] |= parentSet[source];

        for(Vertex *v : childSet[dest]) {
            parentSet[v] << source;
            parentSet[v] |= parentSet[source];
        }

        int weight = (qrand() % 30) - 12;
        edge_tag::CustomWeight tag;
        return new Edge(source, dest, tag, weight);
    }

    return nullptr;
#else
    if(!dest->hasPathTo(source)) {
        int weight = qrand() % 30 - 12;

        edge_tag::CustomWeight tag;
        return new Edge(source, dest, tag, weight);
    }
    QMessageBox::warning(mainWindow, tr("Error!"), tr("Cycle"));
    return nullptr;
#endif
}

Edge* DAGAnimation::newRandomEdge() {
    unsigned size = vertexList.size();
    if(size < 2)
        return nullptr;

    for(Vertex *source : vertexList) {
        for(Vertex *dest : vertexList) {
            if(source == dest)
                continue;

            bool connected = false;
            for(Edge *e : source->outEdges()) {
                if(e->destVertex() == dest) {
                    connected = true;
                    break;
                }
            }
            if(!connected && !dest->hasPathTo(source)) {
                int weight = qrand() % 30 - 12;

                edge_tag::CustomWeight tag;
                return new Edge(source, dest, tag, weight);
            }

        }
    }
    
    return nullptr;
}
