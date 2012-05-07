/*
 * =====================================================================================
 *
 *       Filename:  bellman_ford_visitor.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/03/2012 06:49:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _BELLMANFORD_VISITOR_HPP_
#define _BELLMANFORD_VISITOR_HPP_

#include <boost/config.hpp>

#include <boost/graph/graph_traits.hpp>
//#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>

#include <QList>
#include <QVector>
#include <QTableWidget>
#include <QParallelAnimationGroup>
#include <QAbstractAnimation>
#include <QPropertyAnimation>

#include <utility>
#include <memory>
#include <limits>

#include "vertex.h"
#include "edge.h"
#include "vertexlabel.h"
#include "animation.h"

template<class graph_t, class vertex_d = typename boost::graph_traits<graph_t>::vertex_descriptor,
                        class edge_d = typename boost::graph_traits<graph_t>::edge_descriptor >
    struct BellmanFordVisitor : public boost::bellman_visitor<>
{
    BellmanFordVisitor(vertex_d _source, QList<Vertex*> const &_vertexList, 
                    QVector<std::shared_ptr<VertexLabel>> &_labelVector,        
                    QVector<qint32>& d,
                    QList<std::shared_ptr<QAbstractAnimation>>& _animList) 
        : source(_source), vertexList(_vertexList), labelVector(_labelVector),
            /* predecessor_map(_predecessor_map),*/ old_label(vertexList.size()),
            distance_map(d), animList(_animList) 
            { 

                lastestLabel = labelVector[source].get();
//
//                typename boost::graph_traits<graph_t>::vertex_iterator vi, vi_end;
//                for(boost::tie(vi, vi_end) = boost::vertices(g); vi != vi_end; ++vi) {
//                    initialize_vertex(*vi, g);
//                }

            }

#if 0
    void initialize_vertex(vertex_d u, graph_t const& g) {
        (void) g;

        if(u != source) {
            vertexList[u]->setColor(Vertex::InitColor);
            old_label[u] = { std::numeric_limits<int>::max(), vertexList[source]->getId() };
        } else {
            vertexList[u]->setColor(Vertex::SourceColor);
            old_label[u] = { 0, -1 };
        }
        labelVector[u]->setDist(old_label[u].first);
        labelVector[u]->setPred(old_label[u].second);

        foreach(Edge* e, vertexList[u]->outEdges()) {
            e->setZValue(0);
            e->setState(Edge::Init);
        }
    }
#endif

#if 0
    void examine_vertex(vertex_d u, graph_t const& g) {

        if(u != source) {
            QPropertyAnimation *vertexAnim, *edgeAnim;
            auto prlAnim = std::shared_ptr<QParallelAnimationGroup>
                                (new QParallelAnimationGroup);

            vertex_d v = predecessor_map[u];
            vertexAnim = new QPropertyAnimation(vertexList[u], "color");
            vertexAnim->setDuration(200);
            vertexAnim->setStartValue(vertexList[u]->color());
            vertexAnim->setEndValue(Vertex::VisitedColor);

            foreach(Edge *e, vertexList[v]->outEdges()) {
                if(e->destVertex() == vertexList[u]) {
                    e->setZValue(1000.0);
                    edgeAnim = new QPropertyAnimation(e, "state");
                    edgeAnim->setStartValue(e->state());
                    edgeAnim->setEndValue(Edge::InPath);
                    edgeAnim->setDuration(200);
                    break;
                }
            }

            prlAnim->addAnimation(edgeAnim);
            prlAnim->addAnimation(vertexAnim);
            animList << prlAnim;
        }
    }
#endif

    void edge_relaxed(edge_d e, graph_t const& g) {
//        predecessor_map[g.target(e)] = g.source(e);
        vertex_d target = boost::target(e, g), src = boost::source(e, g);
//        predecessor_map[target] = src;

        VertexLabel *labelItem = labelVector[target].get();
        auto prlAnim =
            std::shared_ptr<QParallelAnimationGroup>(new QParallelAnimationGroup); 

        /* repaint vertex */
        // source vertex
        if(src != source) {
            prlAnim->addAnimation(create(vertexList[src], "color", 
                        Vertex::InitColor, Vertex::DiscoveredColor));
        }

        // target vertex
        prlAnim->addAnimation(create(vertexList[target], "color", 
                    Vertex::InitColor, Vertex::DiscoveredColor));
        /* finish repaint vertex */

        /* repaint edge */
        for(Edge *edge : vertexList[src]->outEdges()) {
            if(edge->destVertex() == vertexList[target]) {
                edge->setZValue(edge->zValue()+2);

                prlAnim->addAnimation(create(edge, "state", Edge::Init, Edge::Relaxed));
                break;
            }
        }
        /* finish repaint edge */

        /* set label */
        std::pair<int, int> new_label = 
                { distance_map[target], vertexList[src]->getId() };

        // set distance
        prlAnim->addAnimation(create(labelItem, "distance",
                    old_label[target].first, new_label.first));

        // set predecessor
        prlAnim->addAnimation(create(labelItem, "predecessor",
                    old_label[target].second, new_label.second));

        old_label[target] = new_label;
        /* finish set label */

        /* select label item */
        // unselect old label
        QPropertyAnimation *unselectLabelAnim =
            new QPropertyAnimation(lastestLabel, "selected");
        unselectLabelAnim->setDuration(200);
        unselectLabelAnim->setStartValue(1);
        unselectLabelAnim->setKeyValueAt(.75, 0);
        unselectLabelAnim->setEndValue(0);
        prlAnim->addAnimation(unselectLabelAnim);

        // select new label
        QPropertyAnimation *selectLabelAnim =
            new QPropertyAnimation(labelItem, "selected");
        selectLabelAnim->setDuration(200);
        selectLabelAnim->setStartValue(0);
        selectLabelAnim->setKeyValueAt(.75, 1);
        selectLabelAnim->setEndValue(1);
        prlAnim->addAnimation(selectLabelAnim);

        lastestLabel = labelItem;
        /* finish select label item */

        animList << prlAnim;
//        animList << std::shared_ptr<QAbstractAnimation>(labelAnim);

//        qDebug() << labelAnim->targetObject();
    }

protected:
    vertex_d source;
    QList<Vertex*> const &vertexList;
//    QTableWidget *labelTable;
    QVector<std::shared_ptr<VertexLabel>>& labelVector;
    VertexLabel* lastestLabel;
//    QVector<vertex_d>& predecessor_map; 
    QVector<std::pair<int,int>> old_label;
    QVector<qint32>& distance_map;
    QList<std::shared_ptr<QAbstractAnimation>>& animList;
};

#endif
