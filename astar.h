/*
 * =====================================================================================
 *
 *       Filename:  astar.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/05/2012 03:11:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _ASTAR_H_
#define _ASTAR_H_

class MainWindow;
class Edge;
class Vertex;

#include "dijkstra.h"
#include "edge.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

typedef AlgorithmTraits<Algorithm::AStar>::graph_t astar_graph_t;

class AStarAnimation : public DijkstraAnimation {
    Q_OBJECT
    typedef astar_graph_t graph_t;
public:
    AStarAnimation(MainWindow *mw);

    Algorithm::AlgType type() const override { return Algorithm::AStar; }
    Edge* newEdge(Vertex*, Vertex*) override;
protected:
    void generateAnimList() override;
};

#endif
