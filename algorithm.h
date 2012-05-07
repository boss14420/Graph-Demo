/*
 * =====================================================================================
 *
 *       Filename:  algorithm.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2012 08:55:01 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

class MainWindow;
class GraphScene;
class Vertex;

class QMenu;
class QToolBar;
class QAction;
class QDockWidget;
class QTableWidget;
class QAbstractAnimation;

#include "edge.h"
//#include "vertexlabel.h"
class VertexLabel;

#include <QVector>
#include <QList>

#include <memory>
#include <exception>

#include <boost/graph/adjacency_list.hpp>
class Algorithm : public QObject {
    Q_OBJECT

public:
    Algorithm(MainWindow *mw);

    enum AlgType { BellmanFord, Dijkstra, DAG, FloydWarshall, BFS, AStar };
    enum WeightPolicy { NoNegativeWeight = 1, CustomWeight = 2, 
        WeightByVertexPosition = 5, NoWeight = 9 };

    void incEdge() { ++nEdge; }
    void decEdge() { --nEdge; }
    void clearEdge() { nEdge = 0; }

    virtual Edge* newEdge(Vertex *, Vertex*) = 0;
    virtual Edge* newRandomEdge() = 0;

    void setSelectedVertex(Vertex*);

    QMenu* getAnimMenu();
    QToolBar* getAnimToolBar();

    static Algorithm* getAlgorithm(AlgType, MainWindow*);

    virtual AlgType type() const = 0;

    friend class GraphScene;
public slots:
    void setSource();
    void setDest();    

    void addVertex(Vertex*);
    void removeVertex(Vertex*);
protected:
    MainWindow *mainWindow;
    QDockWidget *dock;
    QTableWidget *labelTable;
    
    QList<Vertex*> vertexList;
    int nEdge;

    Vertex *selectedVertex;
    Vertex *sourceVertex, *destVertex;

    void timerEvent(QTimerEvent*) override;
    virtual void generateAnimList() = 0;

    QList<std::shared_ptr<QAbstractAnimation>> animList;
    QList<std::shared_ptr<QAbstractAnimation>>::iterator currentAnim;

    QVector<std::shared_ptr<VertexLabel>> labelVector;

    template<typename graph_t> graph_t getGraph();

    void setDelay(int dl) { delay = dl; }
protected slots:
    void finishAnimation();
    void initAllItems();

private:
    bool isPaused;
    bool isStarted;
    bool isAutoPlay;
    bool isForward;

    int delay;
    int timerId;

    QAction *playAction;
    QAction *nextAction;
    QAction *prevAction;
    QAction *stopAction;

    QMenu *animMenu;
    QToolBar *animToolBar;

    void createToolBar();
    void createMenus();
    void createActions();
    void createDockWidget();

private slots:
    void play();
    void next();
    void prev();
    void stop();
};

template<Algorithm::AlgType algtype> struct AlgorithmTraits;

template<> struct AlgorithmTraits<Algorithm::Dijkstra> {
    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
                boost::no_property, EdgeProperty<int> > graph_t;
};
template<> struct AlgorithmTraits<Algorithm::BellmanFord> {
    typedef AlgorithmTraits<Algorithm::Dijkstra>::graph_t graph_t;
};
template<> struct AlgorithmTraits<Algorithm::AStar> {
    typedef AlgorithmTraits<Algorithm::Dijkstra>::graph_t graph_t;
};
template<> struct AlgorithmTraits<Algorithm::DAG> {
    typedef AlgorithmTraits<Algorithm::Dijkstra>::graph_t graph_t;
};
template<> struct AlgorithmTraits<Algorithm::BFS> {
    typedef boost::adjacency_list<boost::listS, boost::vecS, 
                    boost::directedS> graph_t;
};

class TargetFoundException : public std::exception {};
class CycleFoundException : public std::exception {};

#endif
