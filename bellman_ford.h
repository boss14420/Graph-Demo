/*
 * =====================================================================================
 *
 *       Filename:  bellman_ford.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2012 09:06:24 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _BELLMAN_FORD_H_
#define _BELLMAN_FORD_H_

//class MainWindow;
//class Edge;
//class Vertex;

#include "dijkstra.h"
//#include "edge.h"
//
//#include <boost/graph/graph_traits.hpp>
//#include <boost/graph/adjacency_list.hpp>

typedef AlgorithmTraits<Algorithm::BellmanFord>::graph_t bellman_ford_graph_t;

//extern template bellman_ford_graph_t Algorithm::getGraph<bellman_ford_graph_t>();

class BellmanFordAnimation : public DijkstraAnimation {

    Q_OBJECT
    
    typedef bellman_ford_graph_t graph_t;
public:
    BellmanFordAnimation(MainWindow *mw);
    
    Algorithm::AlgType type() const override { return Algorithm::BellmanFord; }
    Edge* newEdge(Vertex*, Vertex*) override;
protected:
    void generateAnimList() override;
};

#endif
