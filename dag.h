/*
 * =====================================================================================
 *
 *       Filename:  dag.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/06/2012 11:36:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _DAG_H_
#define _DAG_H_

class MainWindow;
class Edge;
class Vertex;

#include "algorithm.h"
#include <QHash>
#include <QSet>

typedef AlgorithmTraits<Algorithm::DAG>::graph_t dag_graph_t;

class DAGAnimation : public Algorithm {
    Q_OBJECT
    typedef dag_graph_t graph_t;
public:
    DAGAnimation(MainWindow *mw);

    Edge* newEdge(Vertex*, Vertex*) override;
    Edge* newRandomEdge() override;

    Algorithm::AlgType type() const override { return Algorithm::BFS; }
protected:
    void generateAnimList() override;

#ifdef USE_CHILD_AND_PARENT_SET
private:
    QHash<Vertex*, QSet<Vertex*>> childSet;
    QHash<Vertex*, QSet<Vertex*>> parentSet;
#endif
};

#endif
