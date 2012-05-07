/*
 * =====================================================================================
 *
 *       Filename:  dijkstra.h
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

#ifndef _DIJKSTRA_H_
#define _DIJKSTRA_H_

class MainWindow;
class Edge;
class Vertex;

#include "algorithm.h"
#include "edge.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

typedef AlgorithmTraits<Algorithm::Dijkstra>::graph_t dijkstra_graph_t;

//extern template dijkstra_graph_t Algorithm::getGraph<dijkstra_graph_t>();

class DijkstraAnimation : public Algorithm {

    Q_OBJECT
    typedef dijkstra_graph_t graph_t; 
public:
    DijkstraAnimation(MainWindow *mw);
    
    virtual Edge* newEdge(Vertex*, Vertex*) override;
    virtual Algorithm::AlgType type() const override { return Algorithm::Dijkstra; }
    Edge* newRandomEdge() override;
protected:
    virtual void generateAnimList() override;
};

#endif
