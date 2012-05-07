/*
 * =====================================================================================
 *
 *       Filename:  mainwindow.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/31/2012 07:15:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QtGui/QMainWindow>
#include <QList>
#include <utility>

#include "algorithm.h"

class QGraphicsScene;
class QMenu;
class QToolBar;
class QAction;

class GraphWidget;
class GraphScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Algorithm::AlgType);
    MainWindow(const QString &fileName);

public slots:
    void enableEditing();
    void disableEditing();

protected:
    void closeEvent(QCloseEvent *event); 

private:
    void init(Algorithm::AlgType);
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &);
    bool saveFile(const QString &);
    void setCurrentFile(const QString &);
    QString strippedName(const QString &);
    MainWindow *findMainWindow(const QString &);


    QString curFile;
    bool isUntitled;

    GraphScene *scene;
    GraphWidget *graphwidget;

    QMenu *fileMenu;
    QToolBar *fileToolBar;

    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exportAction;
    QAction *closeAction;
    QAction *exitAction;


    QMenu *editMenu;
    QToolBar *editToolBar;

    QAction *selectItemAction;
    QAction *addRandomVertexAction;
    QAction *addVertexAction;
    QAction *addRandomEdgeAction;
    QAction *addEdgeAction;
    QAction *removeItemAction;
    QAction *clearAllItemsAction;
    QAction *shuffleAction;

//    QMenu *animMenu;
//    QToolBar *animToolBar;
//
//    QAction *dijkstraFindPathAction;
//

    QMenu *viewMenu;
    QAction *zoomOutAction;
    QAction *zoomInAction;

    QMenu *helpMenu;

    QAction *aboutAction;
    QAction *aboutQtAction;


    QMenu *contextMenu;
    QAction *setSourceAction;
    QAction *setTargetAction;

    Algorithm *algorithm;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void exportGraph();

    void selectItem();
    void addVertex();
    void addEdge();

    void about();

    void documentWasModified();
};

#endif
