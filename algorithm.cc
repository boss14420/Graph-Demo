/*
 * =====================================================================================
 *
 *       Filename:  algorithm.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2012 01:33:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "algorithm.h"
#include "mainwindow.h"

#include "dijkstra.h"
#include "bellman_ford.h"
#include "bfs.h"
#include "astar.h"
#include "dag.h"

#include "vertex.h"
#include "edge.h"

#include <QAction>
#include <QHash>
#include <QToolBar>
#include <QMenu>
#include <QDockWidget>
#include <QTableWidget>
#include <QTimerEvent>

#include <QPropertyAnimation>
#include <QDebug>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

Algorithm::Algorithm(MainWindow *_mainWindow) 
            : QObject(_mainWindow), mainWindow(_mainWindow), nEdge(0), 
                selectedVertex(nullptr), sourceVertex(nullptr), destVertex(nullptr),
                isStarted(false), delay(1000)
{
    createActions();
    createMenus();
    createToolBar();

    createDockWidget();
}

void Algorithm::createActions() {
    playAction = new QAction(QIcon(":/images/play.png"), tr("Play"), mainWindow);
    playAction->setShortcut(tr("Ctrl+F5"));
    playAction->setStatusTip(tr("Start algorithm animation"));
    connect(playAction, SIGNAL(triggered()), this, SLOT(play()));

    nextAction = new QAction(QIcon(":/images/next.png"), tr("Next"), mainWindow);
    nextAction->setShortcut(tr("F7"));
    nextAction->setStatusTip(tr("Next animation"));
    connect(nextAction, SIGNAL(triggered()), this, SLOT(next()));

    prevAction = new QAction(QIcon(":/images/prev.png"), tr("Prev"), mainWindow);
    prevAction->setShortcut(tr("F8"));
    prevAction->setStatusTip(tr("Previous animation"));
    connect(prevAction, SIGNAL(triggered()), this, SLOT(prev()));
    prevAction->setEnabled(false);

    stopAction = new QAction(QIcon(":/images/stop.png"), tr("Stop"), mainWindow);
    stopAction->setShortcut(tr("Ctrl+F6"));
    stopAction->setStatusTip(tr("Stop animation"));
    connect(stopAction, SIGNAL(triggered()), this, SLOT(stop()));
    stopAction->setEnabled(false);
}

void Algorithm::createMenus() {
    animMenu = new QMenu(tr("&Animation"), mainWindow);
    animMenu->addAction(prevAction);
    animMenu->addAction(playAction);
    animMenu->addAction(nextAction);
    animMenu->addAction(stopAction);
}

QMenu* Algorithm::getAnimMenu() {
    return animMenu;
}

void Algorithm::createToolBar() {
    animToolBar = new QToolBar(tr("Animation"), mainWindow);
    animToolBar->addAction(prevAction);
    animToolBar->addAction(playAction);
    animToolBar->addAction(nextAction);
    animToolBar->addAction(stopAction);
}

QToolBar* Algorithm::getAnimToolBar() {
    return animToolBar;
}

/* static */
Algorithm* Algorithm::getAlgorithm(AlgType algtype, MainWindow* mainWindow) {
    switch(algtype) {
        case Dijkstra:
            return new DijkstraAnimation(mainWindow);
        case BellmanFord:
            return new BellmanFordAnimation(mainWindow);
        case BFS:
            return new BFSAnimation(mainWindow);
        case AStar:
            return new AStarAnimation(mainWindow);
        case DAG:
            return new DAGAnimation(mainWindow);
        default:
            return nullptr;
    }
}

void Algorithm::createDockWidget() {
    labelTable = new QTableWidget(mainWindow);

    dock = new QDockWidget(tr("Label"), mainWindow);
    dock->setWidget(labelTable);

    mainWindow->addDockWidget(Qt::BottomDockWidgetArea, dock);
}


template<typename graph_t> graph_t Algorithm::getGraph() {}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  getGraph
 *  Description:  getGraph function, specialize for BFS algorithm
 * =====================================================================================
 */

typedef AlgorithmTraits<Algorithm::BFS>::graph_t bfs_graph_t;

template<>
bfs_graph_t Algorithm::getGraph<bfs_graph_t>() {
    typedef std::pair<unsigned, unsigned> edge_t;
    QList<edge_t> edgeList;

    unsigned size = vertexList.size();
    QHash<Vertex*, unsigned> idMap;
    for(unsigned i = 0; i < size; ++i)
        idMap[vertexList[i]] = i;

    for(unsigned i = 0; i < size; ++i) {
        foreach(Edge *e, vertexList[i]->outEdges()) {
            unsigned j = idMap[e->destVertex()];
            edgeList << edge_t(i, j);

//            qDebug() << "Edge (" << i << ", " << j << ") : " << e->property().weight;
        }
    }

    return bfs_graph_t(edgeList.begin(), edgeList.end(), vertexList.size());
}

/* -----  end of function getGraph  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getGraph
 *  Description:  generate boost graph, specialize for Dijkstra algorithm
 * =====================================================================================
 */

typedef AlgorithmTraits<Algorithm::Dijkstra>::graph_t dijkstra_graph_t;

template<>
dijkstra_graph_t Algorithm::getGraph<dijkstra_graph_t>() {
    typedef std::pair<unsigned, unsigned> edge_t;
    QList<edge_t> edgeList;

    typedef typename boost::edge_bundle_type<dijkstra_graph_t>::type property_t;
    QList<property_t> propertyList;

    unsigned size = vertexList.size();
    QHash<Vertex*, unsigned> idMap;
    for(unsigned i = 0; i < size; ++i)
        idMap[vertexList[i]] = i;

    for(unsigned i = 0; i < size; ++i) {
        foreach(Edge *e, vertexList[i]->outEdges()) {
            unsigned j = idMap[e->destVertex()];
            edgeList << edge_t(i, j);
            propertyList << e->property();

//            qDebug() << "Edge (" << i << ", " << j << ") : " << e->property().weight;
        }
    }

    return dijkstra_graph_t(edgeList.begin(), edgeList.end(), 
                    propertyList.begin(), vertexList.size());
}

/* ----- end of function getGraph ------- */


// explicit template instantiation
//template bfs_graph_t Algorithm::getGraph<bfs_graph_t>();
//template dijkstra_graph_t Algorithm::getGraph<dijkstra_graph_t>();

//typedef AlgorithmTraits<Algorithm::BellmanFord>::graph_t bellman_ford_graph_t;
//template bellman_ford_graph_t Algorithm::getGraph<bellman_ford_graph_t>();



void Algorithm::addVertex(Vertex* vertex) {
    vertexList << vertex;
}

void Algorithm::removeVertex(Vertex* vertex) {
    vertexList.removeOne(vertex);
}

void Algorithm::initAllItems() {
    for(Vertex *v : vertexList) {
        if(v != sourceVertex) {
            v->setColor(Vertex::InitColor);
        }
        for(Edge *e : v->outEdges()) {
            e->setState(Edge::Init);
        }
    }
}

void Algorithm::setSource() {
    if(destVertex == selectedVertex)
        destVertex = nullptr;
    if(sourceVertex != selectedVertex) {
        if(sourceVertex != nullptr)
            sourceVertex->setColor(Vertex::InitColor);
        sourceVertex = selectedVertex;
        sourceVertex->setColor(Vertex::SourceColor);
    } else {
        sourceVertex->setColor(Vertex::InitColor);
        sourceVertex = nullptr;
    }
//    emit graphChanged();
}

void Algorithm::setDest() {
    if(sourceVertex == selectedVertex)
        sourceVertex = nullptr;
    if(destVertex != selectedVertex) {
        if(destVertex != nullptr)
            destVertex->setColor(Vertex::InitColor);
        destVertex = selectedVertex;
        destVertex->setColor(Vertex::DestColor);
    } else {
        destVertex->setColor(Vertex::InitColor);
        destVertex = nullptr;
    } 
//    emit graphChanged();
}

void Algorithm::timerEvent(QTimerEvent* event) {
    if(event->timerId() == timerId) {
        if(currentAnim != animList.end()) {
            if(!isPaused) {
//                (*currentAnim)->setDuration(delay);

//                QPropertyAnimation *anim 
//                    = dynamic_cast<QPropertyAnimation*>((*currentAnim).get());
//                if(anim != nullptr)
//                    qDebug() << anim->targetObject();
                (*currentAnim)->start();
            }
        }
    }
}

void Algorithm::finishAnimation() {
    if(currentAnim + 1 == animList.end()) {
        if(isForward) {
            nextAction->setEnabled(false);
            prevAction->setEnabled(true);
            isAutoPlay = false;
//            mainWindow->enableEditing();
        } else {
            nextAction->setEnabled(true);
            prevAction->setEnabled(currentAnim != animList.begin());
        }
        timerId = -2;
    } else if(isAutoPlay) {
        ++currentAnim;
        timerId = startTimer(delay);
    } else {
        nextAction->setEnabled(currentAnim +1  != animList.end());
        prevAction->setDisabled((*currentAnim)->direction() 
                                == QAbstractAnimation::Backward);
//        isPaused = true;
        playAction->setIcon(QIcon(":/images/resume.png"));
        playAction->setText(tr("Resume"));
    }
}

void Algorithm::play() {
    if(!isStarted) {
        mainWindow->disableEditing();
        generateAnimList();
        if(animList.count() == 0) {
            mainWindow->enableEditing();
            return;
        }
        
        // start
        isStarted = true;
        isAutoPlay = true;
        isPaused = false;


        currentAnim = animList.begin();
//        (*currentAnim)->setDuration(delay);
//        (*currentAnim)->start();
        
        playAction->setIcon(QIcon(":/images/pause.png"));
        playAction->setText(tr("Pause"));
        
        nextAction->setEnabled(false);
        prevAction->setEnabled(false);
        stopAction->setEnabled(true);

        timerId = startTimer(delay);
    } else if(!isPaused) {
        // pause
        isPaused = true;

        nextAction->setEnabled(currentAnim != animList.end());
        prevAction->setEnabled(true);

        playAction->setIcon(QIcon(":/images/resume.png"));
        playAction->setText("Resume");
    } else {
        // resume
        isPaused = false;
        isAutoPlay = true;

        timerId = startTimer(delay);

        nextAction->setEnabled(false);
        prevAction->setEnabled(false);

        playAction->setIcon(QIcon(":/images/pause.png"));
        playAction->setText("Pause");
    }
    isForward = true;
}

void Algorithm::next() {
    if(isStarted) {
        if(isForward)
            ++currentAnim;
    } else {
        mainWindow->disableEditing();
        generateAnimList();
        if(animList.count() == 0) {
            mainWindow->enableEditing();
            return;
        }

        currentAnim = animList.begin();
        isStarted = true;
        
        stopAction->setEnabled(true);
    }

    isForward = true;
    isAutoPlay = false;
    isPaused = true;

    playAction->setIcon(QIcon(":/images/resume.png"));
    playAction->setText(tr("Resume"));
    nextAction->setEnabled(false);
    prevAction->setEnabled(false);

//    (*currentAnim)->setDuration(delay);
    (*currentAnim)->setDirection(QAbstractAnimation::Forward);
    (*currentAnim)->start();
}

void Algorithm::prev() {
    if(isStarted) {
        isAutoPlay = false;
        isForward = false;
        isPaused = true;

        playAction->setIcon(QIcon(":/images/resume.png"));
        playAction->setText(tr("Resume"));
        nextAction->setEnabled(false);
        prevAction->setEnabled(false);

//        (*currentAnim)->setDuration(delay);
        (*currentAnim)->setDirection(QAbstractAnimation::Backward);
        (*currentAnim)->start();
        
        if(currentAnim != animList.begin())
            --currentAnim;
    }
}

void Algorithm::stop() {
    killTimer(timerId);
    animList.clear();
    labelVector.clear();

    isStarted = false;

    playAction->setEnabled(true);
    playAction->setIcon(QIcon(":/images/play.png"));
    playAction->setText(tr("Play"));

    nextAction->setEnabled(true);
    prevAction->setEnabled(false);
    stopAction->setEnabled(false);

    mainWindow->enableEditing();
}
