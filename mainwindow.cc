/*
 * =====================================================================================
 *
 *       Filename:  mainwindow.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/31/2012 07:18:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "mainwindow.h"
#include "graphwidget.h"
#include "vertex.h"
#include "edge.h"
#include "graphscene.h"
#include "algorithm.h"
#include "algchooser.h"

//#include <QtGui>
#include <QMessageBox>
#include <QToolBar>
#include <QActionGroup>
#include <QAction>
#include <QMenu>
#include <QIcon>
#include <QFileDialog>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QCloseEvent>
#include <QStatusBar>
#include <QMenuBar>

#include <QApplication>
#include <QSettings>
#include <QDebug>

#include <fstream>

MainWindow::MainWindow(Algorithm::AlgType algtype) {
    init(algtype);
    setCurrentFile("");
}

MainWindow::MainWindow(const QString &fileName) {
    init(Algorithm::Dijkstra);
    loadFile(fileName);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if(maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile() {
//    MainWindow *other = new MainWindow(Algorithm::Dijkstra);
//    other->move(x() + 40, y() + 40);
//    other->show();
    AlgorithmChooserDialog dialog;
    if(dialog.exec() == QDialog::Accepted) {
        MainWindow *mainWindow = dialog.createMainWindow();
        mainWindow->move(x()+40, y()+40);
        mainWindow->show();
//        qDebug() << "Hello";
    }
}

void MainWindow::open() {
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Select graph file"), tr("."),
            "GraphViz dot files (*.dot);;All files (*.*)");
    if (!fileName.isEmpty()) {
        MainWindow *existing = findMainWindow(fileName);
        if (existing) {
            existing->show();
            existing->raise();
            existing->activateWindow();
            return;
        }

        if (isUntitled && scene->items().empty()
                && !isWindowModified()) {
            loadFile(fileName);
        } else {
            MainWindow *other = new MainWindow(fileName);
            if (other->isUntitled) {
                delete other;
                return;
            }
            other->move(x() + 40, y() + 40);
            other->show();
        }
    }
}

bool MainWindow::save() {
    if(isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as ..."), curFile,
            "GraphViz dot files (*.dot);;All files (*.*)");

    if(fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void MainWindow::exportGraph() {
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Export to Image"), "",
            tr("JPEG (*.jpg);;"
                "Windows Bitmap (*.bmp);;"
                "Portable Network Graphics (*.png);;"
                "Portable Pixmap (*.ppm);;"
                "Tagged Image File Format (*.tiff);;"
                "X11 Bitmap (*.xbm);;"
                "X11 Pixmap (*.xpm);;"
                "Images (*.bmp *.jpg *.jpeg *.png *.ppm *.tiff *.xbm *.xpm)"));
    
    if(fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("graph"),
                tr("Can't open file %1:\n%2.")
                .arg(fileName)
                .arg(file.errorString()));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QImage img(graphwidget->width(), graphwidget->height(), 
            QImage::Format_ARGB32_Premultiplied);
    QPainter p(&img);
    graphwidget->render(&p);
    p.end();
    img.save(fileName);
    QApplication::restoreOverrideCursor();
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About the program"),
            tr("This program demonstrates Shortest-Path algorithms in graphs."));
}

void MainWindow::documentWasModified() {
    setWindowModified(true);
}

void MainWindow::init(Algorithm::AlgType algtype) {
    setAttribute(Qt::WA_DeleteOnClose);

    isUntitled = true;
    
    // algorithmanimation object
    algorithm = Algorithm::getAlgorithm(algtype, this);

    // context menu
    contextMenu = new QMenu(this);
    
    // Scene object init
    scene = new GraphScene(contextMenu, algorithm, this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
//    scene->setSceneRect(-1500, -1000, 3000, 2000);

    // Graph widget
    graphwidget = new GraphWidget(this);
    graphwidget->setScene(scene);
    setCentralWidget(graphwidget);

    scene->setSceneRect(-width()/2, -height()/2, width(), height());

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();
    
    connect(scene, SIGNAL(graphChanged()), 
            this, SLOT(documentWasModified()));

    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::createActions() {
    // File action
    newAction = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAction->setShortcuts(QKeySequence::New);
    newAction->setStatusTip(tr("Open new window"));
    connect(newAction, SIGNAL(triggered()), this, SLOT(newFile()));

    openAction = new QAction(QIcon(":/images/open.png"), tr("&Open"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open file"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    saveAction = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save file"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAction = new QAction(tr("Save &As"), this);
    saveAsAction->setShortcuts(QKeySequence::SaveAs);
    saveAsAction->setStatusTip(tr("Save as ..."));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

    exportAction = new QAction(tr("&Export"), this);
    exportAction->setShortcut(tr("Ctrl+X"));
    exportAction->setStatusTip(tr("Export to image"));
    connect(exportAction, SIGNAL(triggered()), this, SLOT(exportGraph()));

    closeAction = new QAction(tr("&Close"), this);
    closeAction->setShortcut(tr("Ctrl+W"));
    closeAction->setStatusTip(tr("Close this window"));
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    // Edit graph action
    selectItemAction = new QAction(QIcon(":/images/pointer.png"), tr("&Select"), this);
    selectItemAction->setStatusTip(tr("Selection tool"));
    selectItemAction->setCheckable(true);
    connect(selectItemAction, SIGNAL(triggered()), this, SLOT(selectItem()));

    addEdgeAction = new QAction(QIcon(":/images/edge.png"), tr("&Draw Edge"), this);
    addEdgeAction->setStatusTip(tr("Draw edge tool"));
    addEdgeAction->setCheckable(true);
    connect(addEdgeAction, SIGNAL(triggered()), this, SLOT(addEdge()));

    addVertexAction = new QAction(QIcon(":/images/vertex.png"), tr("D&raw Vertex"), this);
    addVertexAction->setStatusTip(tr("Draw vertex tool"));
    addVertexAction->setCheckable(true);
    connect(addVertexAction, SIGNAL(triggered()), this, SLOT(addVertex()));

    QActionGroup *modeActionGroup = new QActionGroup(this);
    modeActionGroup->addAction(selectItemAction);
    modeActionGroup->addAction(addVertexAction);
    modeActionGroup->addAction(addEdgeAction);
    selectItemAction->setChecked(true);

    addRandomVertexAction = new QAction(QIcon(":/images/rdvertex.png"),
            tr("Random &Vertex"), this);
    addRandomVertexAction->setShortcut(tr("Ctrl+V"));
    addRandomVertexAction->setStatusTip(tr("Add random vertex"));
    connect(addRandomVertexAction, SIGNAL(triggered()), scene, SLOT(addRandomVertex()));

    addRandomEdgeAction = new QAction(QIcon(":/images/rdedge.png"),
            tr("Random &Edge"), this);
    addRandomEdgeAction->setShortcut(tr("Ctrl+E"));
    addRandomEdgeAction->setStatusTip(tr("Add random edge"));
    connect(addRandomEdgeAction, SIGNAL(triggered()), scene, SLOT(addRandomEdge()));

    removeItemAction = new QAction(QIcon(":/images/delete.png"), tr("Remove &Items"), this);
    removeItemAction->setShortcuts(QKeySequence::Delete);
    removeItemAction->setStatusTip(tr("Remove selected items"));
    connect(removeItemAction, SIGNAL(triggered()), scene, SLOT(removeSelectedItems()));

    clearAllItemsAction = new QAction(QIcon(":/images/clear.png"), tr("&Clear"), this);
    clearAllItemsAction->setStatusTip(tr("Clear the graph"));
    connect(clearAllItemsAction, SIGNAL(triggered()), scene, SLOT(clearAllItems()));

    shuffleAction = new QAction(tr("&Shuffle"), this);
    shuffleAction->setStatusTip(tr("Shuffle the graph"));
    shuffleAction->setShortcut(tr("Ctrl+Space"));
    connect(shuffleAction, SIGNAL(triggered()), graphwidget, SLOT(shuffle()));

    // Animation action
//    dijkstraFindPathAction = new QAction(tr("Dijkstra"), this);
//    dijkstraFindPathAction->setStatusTip(tr("Find the shortest path by Dijkstra algorithm"));
//    dijkstraFindPathAction->setShortcut(tr("Ctrl+F5"));
//    connect(dijkstraFindPathAction, SIGNAL(triggered()), scene, SLOT(dijkstraFindPath()));

    // View actions

    zoomInAction = new QAction(tr("Zoom &in"), this);
    zoomInAction->setStatusTip(tr("Zoom in"));
    zoomInAction->setShortcuts(QKeySequence::ZoomIn);
    connect(zoomInAction, SIGNAL(triggered()), graphwidget, SLOT(zoomIn()));

    zoomOutAction = new QAction(tr("Zoom &in"), this);
    zoomOutAction->setStatusTip(tr("Zoom in"));
    zoomOutAction->setShortcuts(QKeySequence::ZoomOut);
    connect(zoomOutAction, SIGNAL(triggered()), graphwidget, SLOT(zoomOut()));

    // Help action
    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setStatusTip(tr("About the program"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setStatusTip(tr("About Qt"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    // ContextMenu action
    setSourceAction = new QAction(tr("Set as source"), this);
    setSourceAction->setCheckable(true);
    connect(setSourceAction, SIGNAL(triggered()), scene, SLOT(setSource()));

    setTargetAction = new QAction(tr("Set as target"), this);
    setTargetAction->setCheckable(true);
    connect(setTargetAction, SIGNAL(triggered()), scene, SLOT(setDest()));
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exportAction);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(selectItemAction);
    editMenu->addAction(addVertexAction);
    editMenu->addAction(addEdgeAction);
    editMenu->addSeparator();
    editMenu->addAction(addRandomVertexAction);
    editMenu->addAction(addRandomEdgeAction);
    editMenu->addSeparator();
    editMenu->addAction(removeItemAction);
    editMenu->addAction(clearAllItemsAction);
    editMenu->addSeparator();
    editMenu->addAction(shuffleAction);

//    animMenu = menuBar()->addMenu(tr("&Animation"));
//    animMenu->addAction(dijkstraFindPathAction);
    menuBar()->addMenu(algorithm->getAnimMenu());

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(zoomInAction);
    viewMenu->addAction(zoomOutAction);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);

    // Context menu
    contextMenu->addAction(setSourceAction);
    contextMenu->addAction(setTargetAction);
    contextMenu->addSeparator();
    contextMenu->addAction(removeItemAction);
}

void MainWindow::createToolBars() {
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);

    editToolBar = addToolBar(tr("Edit toolbar"));
    editToolBar->addAction(selectItemAction);
    editToolBar->addAction(addVertexAction);
    editToolBar->addAction(addEdgeAction);


    editToolBar->addSeparator();
    editToolBar->addAction(addRandomVertexAction);
    editToolBar->addAction(addRandomEdgeAction);
    editToolBar->addSeparator();
    editToolBar->addAction(removeItemAction);
    editToolBar->addAction(clearAllItemsAction);

//    animToolBar = addToolBar(tr("Animation"));
//    animToolBar->addAction(dijkstraFindPathAction);
    addToolBar(algorithm->getAnimToolBar()); 
}

void MainWindow::createStatusBar() {
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings() {
    QSettings settings;
    QPoint pos = settings.value("pos", QPoint(100, 100)).toPoint();
    QSize size = settings.value("size", QSize(600, 600)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::writeSettings() {
    QSettings settings;
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool MainWindow::maybeSave() {
    if (scene->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("graph"),
                tr("The graph has been modified.\n"
                    "Do you want to save your changes ?"),
                QMessageBox::Save | QMessageBox::Discard
                | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("graph"),
                tr("Cannot read file %1:\n%2.")
                .arg(fileName)
                .arg(file.errorString()));
        return;
    }

//    QTextStream in(&file);
    std::ifstream in(fileName.toStdString().c_str());
    QApplication::setOverrideCursor(Qt::WaitCursor);

    try {
//        textEdit->setPlainText(in.readAll());
        scene->loadFile(in);
    } catch(std::exception &ex) {
        QApplication::restoreOverrideCursor();
        statusBar()->showMessage(QString(ex.what()),2000);
        QMessageBox::warning(this, tr("graph"), QString(ex.what()));
        return;
    }

    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);
    statusBar()->showMessage(tr(""), 2000);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("SDI"),
                tr("Cannot write file %1:\n%2.")
                .arg(fileName)
                .arg(file.errorString()));
        return false;
    }

//    QTextStream out(&file);
    std::ofstream out(fileName.toStdString().c_str());
    QApplication::setOverrideCursor(Qt::WaitCursor);
//    out << textEdit->toPlainText();
    scene->saveToFile(out);
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    static int sequenceNumber = 1;

    isUntitled = fileName.isEmpty();
    if (isUntitled) {
        curFile = tr("graph%1.dot").arg(sequenceNumber++);
    } else {
        curFile = QFileInfo(fileName).canonicalFilePath();
    }

    scene->setModified(false);
    setWindowModified(false);
    setWindowFilePath(curFile);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

MainWindow *MainWindow::findMainWindow(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QWidget *widget, qApp->topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin && mainWin->curFile == canonicalFilePath)
            return mainWin;
    }
    return 0;
}

void MainWindow::selectItem() {
    scene->setMode(GraphScene::MoveItem);
}

void MainWindow::addVertex() {
    scene->setMode(GraphScene::InsertVertex);
}

void MainWindow::addEdge() {
    scene->setMode(GraphScene::InsertEdge);
}

void MainWindow::disableEditing() {
    addRandomEdgeAction->setEnabled(false);
    addRandomVertexAction->setEnabled(false);
    clearAllItemsAction->setEnabled(false);
    removeItemAction->setEnabled(false);

    selectItemAction->setChecked(true);
//    selectItem();

    addVertexAction->setEnabled(false);
    addEdgeAction->setEnabled(false);

    setSourceAction->setEnabled(false);
    setTargetAction->setEnabled(false);

    switch(algorithm->type()) {
        case Algorithm::AStar:
            shuffleAction->setEnabled(false);
            for(QGraphicsItem *item :  scene->items()) {
                if(item->type() == Vertex::Type) {
                    item->setFlag(QGraphicsItem::ItemIsMovable, false);
                }
            }
            break;
        case Algorithm::Dijkstra:
        case Algorithm::BellmanFord:
        case Algorithm::DAG:
        case Algorithm::FloydWarshall:
            for(QGraphicsItem *item : scene->items()) {
                if(item->type() == Edge::Type) {
                    /// disable edit weighttextitem
                    item->childItems()[0]->setAcceptedMouseButtons(Qt::NoButton);
                }
            }
            break;
        case Algorithm::BFS:
            break;
    }
}

void MainWindow::enableEditing() {
    addRandomEdgeAction->setEnabled(true);
    addRandomVertexAction->setEnabled(true);
    clearAllItemsAction->setEnabled(true);
    removeItemAction->setEnabled(true);

    addVertexAction->setEnabled(true);
    addEdgeAction->setEnabled(true);

    setSourceAction->setEnabled(true);
    setTargetAction->setEnabled(true);

    switch(algorithm->type()) {
        case Algorithm::AStar:
            shuffleAction->setEnabled(true);
            for(QGraphicsItem *item :  scene->items()) {
                if(item->type() == Vertex::Type) {
                    item->setFlag(QGraphicsItem::ItemIsMovable, true);
                }
            }
            break;
        case Algorithm::Dijkstra:
        case Algorithm::BellmanFord:
        case Algorithm::DAG:
        case Algorithm::FloydWarshall:
            for(QGraphicsItem *item : scene->items()) {
                if(item->type() == Edge::Type) {
                    /// disable edit weighttextitem
                    item->childItems()[0]->setAcceptedMouseButtons(Qt::LeftButton);
                }
            }
            break;
        case Algorithm::BFS:
            break;
    }
}
