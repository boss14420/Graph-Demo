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

#endif
