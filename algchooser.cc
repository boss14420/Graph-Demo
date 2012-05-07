/*
 * =====================================================================================
 *
 *       Filename:  algchooser.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/16/2012 12:51:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "algchooser.h"
#include "mainwindow.h"

#include <QPushButton>
#include <QCheckBox>
#include <QAction>
//#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>

AlgorithmChooserDialog::AlgorithmChooserDialog(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    // CheckBoxes
//    QVBoxLayout *ckbLayout = new QVBoxLayout;
//    weightPolicyButtonGroup = new QButtonGroup(this);
//
//    noWeightRadioButton = new QRadioButton(tr("No Weight Graph (weight = 1)"), this);
//    ckbLayout->addWidget(noWeightRadioButton);
//    weightPolicyButtonGroup->addButton(noWeightRadioButton);
//
//    weightByPosRadioButton = new QRadioButton(tr("Weighted By Vertex's Position"), this);
//    ckbLayout->addWidget(weightByPosRadioButton);
//    weightPolicyButtonGroup->addButton(weightByPosRadioButton);
//
//
//    customWeightRadioButton = new QRadioButton(tr("Custom weight"), this);
//    ckbLayout->addWidget(customWeightRadioButton);
//    weightPolicyButtonGroup->addButton(customWeightRadioButton);
//    customWeightRadioButton->setChecked(true);
//
//    noNegWeightCheckBox = new QCheckBox(tr("No Negative-Weight Edge Graph"), this);
//    QHBoxLayout *noNegHLayout = new QHBoxLayout;
//    noNegHLayout->addSpacing(15);
//    noNegHLayout->addWidget(noNegWeightCheckBox);
//    ckbLayout->addLayout(noNegHLayout);
//    connect(customWeightRadioButton, SIGNAL(toggled(bool)),
//                noNegWeightCheckBox, SLOT(setEnabled(bool)));
//
//    noCircleCheckBox = new QCheckBox(tr("No Circle Graph"), this);
//    ckbLayout->addWidget(noCircleCheckBox);
//    
//    ckbLayout->addStretch(1);
//    QGroupBox *ckbGroupBox = new QGroupBox(tr("Graph type:"), this);
//    ckbGroupBox->setLayout(ckbLayout);
//
    // Algorithm
    algorithmListButtonGroup = new QButtonGroup(this);
    QVBoxLayout *algoLstLayout = new QVBoxLayout;

    bfsRadioButton = new QRadioButton(tr("Breadth-first-search (BFS)"), this);
    algorithmListButtonGroup->addButton(bfsRadioButton);
    algoLstLayout->addWidget(bfsRadioButton);
    bfsRadioButton->setChecked(true); // default algo
//    connect(noWeightRadioButton, SIGNAL(toggled(bool)),
//            bfsRadioButton, SLOT(setEnabled(bool)));
//    bfsRadioButton->setEnabled(false);

    bellmanFordRadioButton = new QRadioButton(tr("Bellman-Ford"), this);
    algorithmListButtonGroup->addButton(bellmanFordRadioButton);
    algoLstLayout->addWidget(bellmanFordRadioButton);

    dijkstraRadioButton = new QRadioButton(tr("Dijkstra"), this);
    algorithmListButtonGroup->addButton(dijkstraRadioButton);
    algoLstLayout->addWidget(dijkstraRadioButton);
//    connect(noNegWeightCheckBox, SIGNAL(toggled(bool)), 
//            dijkstraRadioButton, SLOT(setEnabled(bool)));
//    connect(weightByPosRadioButton, SIGNAL(toggled(bool)), 
//            dijkstraRadioButton, SLOT(setEnabled(bool)));
//    connect(noWeightRadioButton, SIGNAL(toggled(bool)), 
//            dijkstraRadioButton, SLOT(setEnabled(bool)));
//    dijkstraRadioButton->setEnabled(false);

    aStarRadioButton = new QRadioButton(tr("A* search"), this);
    algorithmListButtonGroup->addButton(aStarRadioButton);
    algoLstLayout->addWidget(aStarRadioButton);
//    connect(noNegWeightCheckBox, SIGNAL(toggled(bool)),
//            aStarRadioButton, SLOT(setEnabled(bool)));
//    connect(weightByPosRadioButton, SIGNAL(toggled(bool)),
//            aStarRadioButton, SLOT(setEnabled(bool)));
//    connect(noWeightRadioButton, SIGNAL(toggled(bool)),
//            aStarRadioButton, SLOT(setEnabled(bool)));
//    aStarRadioButton->setEnabled(false);


    dagRadioButton = new QRadioButton(tr("Directed ACyclic Graph (DAG) search"), this);
    algorithmListButtonGroup->addButton(dagRadioButton);
    algoLstLayout->addWidget(dagRadioButton);
//    connect(noCircleCheckBox, SIGNAL(toggled(bool)),
//            dagRadioButton, SLOT(setEnabled(bool)));
//    dagRadioButton->setEnabled(false);

    floydWarshallRadioButton = new QRadioButton(tr("Floyd-Warshall"), this);
    algorithmListButtonGroup->addButton(floydWarshallRadioButton);
    algoLstLayout->addWidget(floydWarshallRadioButton);
    floydWarshallRadioButton->setEnabled(false);

    QGroupBox *algoLstGroupBox = new QGroupBox(tr("Algorithm"), this);
    algoLstGroupBox->setLayout(algoLstLayout);

    // push button
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    
    okButton = new QPushButton(tr("OK"), this);
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    buttonLayout->addWidget(okButton);

    cancelButton = new QPushButton(tr("Cancel"), this);
    buttonLayout->addWidget(cancelButton);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    // main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
//    mainLayout->addWidget(ckbGroupBox);
    mainLayout->addWidget(algoLstGroupBox);
    mainLayout->addSpacing(15);
    mainLayout->addLayout(buttonLayout);
//    setLayout(mainLayout);

    setWindowTitle(tr("Choose algorithm"));
}

//void AlgorithmChooserDialog::noWeightStateChange(int state) {
//    m_type &= NOWEIGHT_CLEAR;
//    if(state == Qt::Checked)
//        m_type |= NOWEIGHT_CHECKED;
//    update();
//}


MainWindow* AlgorithmChooserDialog::createMainWindow() {
    // Algorithm
    Algorithm::AlgType algo;
    if(bellmanFordRadioButton->isChecked())
        algo = Algorithm::BellmanFord;
    else if(dijkstraRadioButton->isChecked())
        algo = Algorithm::Dijkstra;
    else if(dagRadioButton->isChecked())
        algo = Algorithm::DAG;
    else if(floydWarshallRadioButton->isChecked())
        algo = Algorithm::FloydWarshall;
    else if(bfsRadioButton->isChecked())
        algo = Algorithm::BFS;
    else
        algo = Algorithm::AStar;
//
//    // Graph type
//    Algorithm::WeightPolicy gtype;
//    if(customWeightRadioButton->isChecked()) {
//        gtype = Algorithm::CustomWeight;
//        if(noNegWeightCheckBox)
//            gtype |= Algorithm::NoNegativeWeight;
//    } else if(weightByPosRadioButton->isChecked())
//        gtype = Algorithm::WeightByVertexPosition;
//    else
//        gtype = Algorithm::NoWeight;
//
    return new MainWindow(algo);
//    return nullptr;
}
