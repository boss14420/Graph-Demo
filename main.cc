/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/31/2012 06:27:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <QtGui>

#include "mainwindow.h"
#include "algorithm.h"
#include "algchooser.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(graphdemo);
    QApplication app(argc, argv);
	app.setApplicationName("Graph Demo");

	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));

//    MainWindow *mainWindow = new MainWindow(Algorithm::Dijkstra);
//    mainWindow->show();
    AlgorithmChooserDialog dialog;
    if(dialog.exec() == QDialog::Accepted) {
        MainWindow *mainWindow = dialog.createMainWindow();
        mainWindow->show();
//        qDebug() << "Hello";
    }
    return app.exec();
}
