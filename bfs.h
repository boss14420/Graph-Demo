/*
 * =====================================================================================
 *
 *       Filename:  bfs.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/02/2012 09:06:24 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _BFS_H
#define _BFS_H

class MainWindow;
class Edge;
class Vertex;

#include "dijkstra.h"
#include "edge.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

typedef AlgorithmTraits<Algorithm::BFS>::graph_t bfs_graph_t;

//extern template bfs_graph_t Algorithm::getGraph<bfs_graph_t>();

class BFSAnimation : public DijkstraAnimation {

    Q_OBJECT
    typedef bfs_graph_t graph_t; 
public:
    BFSAnimation(MainWindow *mw);
    
    Algorithm::AlgType type() const override { return Algorithm::BFS; }
    Edge* newEdge(Vertex*, Vertex*) override;
protected:
    void generateAnimList() override;
};

#endif
