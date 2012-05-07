/*
 * =====================================================================================
 *
 *       Filename:  dag_visitor.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/07/2012 01:02:15 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _DAG_VISITOR_HPP_
#define _DAG_VISITOR_HPP_

#include <boost/config.hpp>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
//#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>

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
    struct DFSVisitor : public boost::dfs_visitor<>
{
    DFSVisitor(vertex_d _source, vertex_d _target, 
                QList<Vertex*>& _vertexList, QList<vertex_d>& _topoRevOrder,
                QList<std::shared_ptr<QAbstractAnimation>>& _animList)
        : source(_source), target(_target), 
          vertexList(_vertexList), topoRevOrder(_topoRevOrder),
          animList(_animList)
    {
    }

    void initialize_vertex(vertex_d v, graph_t const& g) {
        if(v != source && v != target) {
            vertexList[v]->setColor(Vertex::InitColor);
        }
        vertexList[v]->setTopoOrder(-1);
    }

    void discover_vertex(vertex_d v, graph_t const& g) {
        if(v == source || v == target)
            return;

        auto anim = std::shared_ptr<QAbstractAnimation>(
                create(vertexList[v], "color", 
                    Vertex::InitColor, Vertex::DiscoveredColor));
        animList << anim;
    }

    void finish_vertex(vertex_d v, graph_t const& g) {
        topoRevOrder << v;
    }

    void back_edge(edge_d e, graph_t const& g) {
        throw CycleFoundException();
    }

private:
    vertex_d source, target;
    QList<Vertex*>& vertexList;
    QList<vertex_d>& topoRevOrder;
    QList<std::shared_ptr<QAbstractAnimation>>& animList;
};


template<class graph_t, class vertex_d = typename boost::graph_traits<graph_t>::vertex_descriptor,
                        class edge_d = typename boost::graph_traits<graph_t>::edge_descriptor >
    struct DAGVisitor : public boost::dijkstra_visitor<>
{
    DAGVisitor(vertex_d _source, vertex_d _target, QList<Vertex*> const &_vertexList, 
                    QVector<std::shared_ptr<VertexLabel>> &_labelVector,        
                    QVector<qint32>& _distance_map,
//                    QVector<vertex_d>& _predecessor_map,
//                    QVector<qint32>& _color_map,
                    QList<std::shared_ptr<QAbstractAnimation>>& _animList) 
        : source(_source), target(_target), vertexList(_vertexList), 
            labelVector(_labelVector),
            distance_map(_distance_map), 
            predecessor_map(vertexList.size(), -1), 
//            predecessor_map(_predecessor_map), 
//            color_map(_color_map),
            old_label(vertexList.size()),
            animList(_animList) 
    { 
                lastestLabel = labelVector[source].get();
    }

    void initialize_vertex(vertex_d u, graph_t const& g) {
        (void) g;


        if(u != source && u != target) {
            vertexList[u]->setColor(Vertex::InitColor);
            old_label[u] = { std::numeric_limits<int>::max(), -1 };
        } else if(u == source) {
            vertexList[u]->setColor(Vertex::SourceColor);
            old_label[u] = { 0, -1 };
        } else {
            // u is target
            vertexList[u]->setColor(Vertex::DestColor);
            old_label[u] = { std::numeric_limits<int>::max(), -1 };
        }

        labelVector[u]->setDist(old_label[u].first);
        labelVector[u]->setPred(old_label[u].second);

        foreach(Edge* e, vertexList[u]->outEdges()) {
            e->setZValue(0);
            e->setState(Edge::Init);
        }
    }

    void examine_vertex(vertex_d v, graph_t const& g) {

        if(v != source) {
            auto prlAnim = std::shared_ptr<QParallelAnimationGroup>
                                (new QParallelAnimationGroup);

            if(v != target) {
                prlAnim->addAnimation(
                        create(vertexList[v], "color",
                            Vertex::DiscoveredColor, Vertex::VisitedColor));
            }

            vertex_d pv = predecessor_map[v];
            foreach(Edge *e, vertexList[pv]->outEdges()) {
                if(e->destVertex() == vertexList[v]) {
                    e->setZValue(1000.0);
                    prlAnim->addAnimation(create(e, "state", Edge::Relaxed, Edge::InPath));
                    break;
                }
            }

            animList << prlAnim;

            if(v == target)
                throw TargetFoundException();
        }
    }

//    void discover_vertex(vertex_d v, graph_t const& g) {
    void edge_relaxed(edge_d e, graph_t const& g) {
        vertex_d v = boost::target(e, g), pv = boost::source(e, g);
        predecessor_map[v] = pv;

//        if(v == source)
//            return;

//        vertex_d pv = predecessor_map[v];

        VertexLabel *labelItem = labelVector[v].get();
        auto prlAnim =
            std::shared_ptr<QParallelAnimationGroup>(new QParallelAnimationGroup); 

        /* set label */
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

//        if(color_map[v] == boost::color_traits<qint32>::white()) {
            /* repaint vertex to gray color */
            if(v != target) {
                prlAnim->addAnimation(create(vertexList[v], "color", 
                            Vertex::InitColor, Vertex::DiscoveredColor));
            }
            /* finish repaint vertex*/

            /* repaint edge */
            foreach(Edge *edge, vertexList[pv]->outEdges()) {
                if(edge->destVertex() == vertexList[v]) {
                    edge->setZValue(1000.0);

                    prlAnim->addAnimation(create(edge, "state",
                                Edge::Init, Edge::Relaxed));
                    break;
                }
            }
            /* finish repaint */
//        }

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
    vertex_d source, target;
    QList<Vertex*> const &vertexList;
//    QTableWidget *labelTable;
    QVector<std::shared_ptr<VertexLabel>>& labelVector;
    VertexLabel* lastestLabel;
    QVector<qint32>& distance_map;
    QVector<vertex_d> predecessor_map; 
//    QVector<vertex_d>& predecessor_map; 
//    QVector<qint32>& color_map;
    QVector<std::pair<int,int>> old_label;
    QList<std::shared_ptr<QAbstractAnimation>>& animList;
};


#endif
