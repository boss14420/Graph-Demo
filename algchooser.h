/*
 * =====================================================================================
 *
 *       Filename:  algchooser.h
 *
 *    Description:  Algorithm chooser dialog
 *
 *        Version:  1.0
 *        Created:  04/16/2012 03:48:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _ALGORITHMCHOOSERDIALOG_H_
#define _ALGORITHMCHOOSERDIALOG_H_

class QPushButton;
class QCheckBox;
class QRadioButton;
class QButtonGroup;
class QAction;
//class QComboBox;

class MainWindow;

#include <QDialog>

class AlgorithmChooserDialog : public QDialog {
    Q_OBJECT

public:
    AlgorithmChooserDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
//    Algorithm getAlgorithm() const;
    MainWindow *createMainWindow();

private:
//    QCheckBox *noCircleCheckBox;    
//    QCheckBox *noNegWeightCheckBox;
//    
//    QRadioButton *noWeightRadioButton;
//    QRadioButton *weightByPosRadioButton;
//    QRadioButton *customWeightRadioButton;

    QButtonGroup *weightPolicyButtonGroup;

//    QComboBox *algoListComboBox;
    QButtonGroup *algorithmListButtonGroup;
    QRadioButton *bellmanFordRadioButton;
    QRadioButton *dijkstraRadioButton;
    QRadioButton *dagRadioButton;
    QRadioButton *floydWarshallRadioButton;
    QRadioButton *bfsRadioButton;
    QRadioButton *aStarRadioButton;

    // Push button
    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif
