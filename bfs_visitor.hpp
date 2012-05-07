/*
 * =====================================================================================
 *
 *       Filename:  bfs_visitor.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/02/2012 06:49:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _BFS_VISITOR_HPP_
#define _BFS_VISITOR_HPP_

#include <boost/config.hpp>

#include <boost/graph/graph_traits.hpp>
//#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>

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
    struct BFSVisitor : public boost::bfs_visitor<>
{
    BFSVisitor(vertex_d _source, vertex_d _target, QList<Vertex*> const &_vertexList, 
                    QVector<std::shared_ptr<VertexLabel>> &_labelVector,        
                    QList<std::shared_ptr<QAbstractAnimation>>& _animList) 
        : source(_source), target(_target), 
            vertexList(_vertexList), 
            labelVector(_labelVector),
            distance_map(vertexList.size(), std::numeric_limits<qint32>::max()),
            predecessor_map(vertexList.size(), -1), old_label(vertexList.size()),
            animList(_animList) 
    { 
        distance_map[source] = 0;
        lastestLabel = labelVector[source].get();
    }

    void initialize_vertex(vertex_d v, graph_t const& g) {
        (void) g;

        if(v != source && v != target) {
            vertexList[v]->setColor(Vertex::InitColor);
            old_label[v] = { std::numeric_limits<int>::max(), -1 };
        } else if (v == source) {
            vertexList[v]->setColor(Vertex::SourceColor);
            old_label[v] = { 0, -1 };
        } else {
            vertexList[v]->setColor(Vertex::DestColor);
            old_label[v] = { std::numeric_limits<int>::max(), -1 };
        }
        labelVector[v]->setDist(old_label[v].first);
        labelVector[v]->setPred(old_label[v].second);

        foreach(Edge* e, vertexList[v]->outEdges()) {
            e->setZValue(0);
            e->setState(Edge::Init);
        }
    }
    
    void tree_edge(edge_d e, graph_t const& g) {
        vertex_d pv = boost::source(e, g), v = boost::target(e, g);

        predecessor_map[v] = pv;
        distance_map[v] = distance_map[pv] + 1;
    }


    void discover_vertex(vertex_d v, graph_t const& g) {
        if(v == source)
            return;

        auto prlAnim = std::shared_ptr<QParallelAnimationGroup>
            (new QParallelAnimationGroup);


        if(v != target) {
            /* repaint vertex */
            prlAnim->addAnimation(create(vertexList[v], "color",
                        Vertex::InitColor, Vertex::VisitedColor));
            /* finish repaint vertex */
        }

        /* repaint edge */
        vertex_d pv = predecessor_map[v];

        foreach(Edge *e, vertexList[pv]->outEdges()) {
            if(e->destVertex() == vertexList[v]) {
                e->setZValue(1000.0);

                prlAnim->addAnimation(create(e, "state", Edge::Init, Edge::InPath));
                break;
            }
        }
        /* finish repaint vertex */

        /* set label */
        VertexLabel *labelItem = labelVector[v].get();
        std::pair<int, int> new_label = 
                { distance_map[v], vertexList[pv]->getId() };

        // set distance
        prlAnim->addAnimation(create(labelItem, "distance",
                    old_label[v].first, new_label.first));

        // set predecessor
        prlAnim->addAnimation(create(labelItem, "predecessor",
                    old_label[v].second, new_label.second));

        old_label[v] = new_label;
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

        // finish if found target
        if(v == target)
            throw TargetFoundException();
    }

protected:
    vertex_d source, target;
    QList<Vertex*> const &vertexList;
    QVector<std::shared_ptr<VertexLabel>>& labelVector;
    QVector<qint32> distance_map;
    QVector<vertex_d> predecessor_map; 
    QVector<std::pair<int,int>> old_label;
    QList<std::shared_ptr<QAbstractAnimation>>& animList;
    VertexLabel* lastestLabel;
};

#endif
