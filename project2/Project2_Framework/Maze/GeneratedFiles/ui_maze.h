/********************************************************************************
** Form generated from reading UI file 'maze.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAZE_H
#define UI_MAZE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QWidget>
#include <openglwidget.h>

QT_BEGIN_NAMESPACE

class Ui_MazeClass
{
public:
    QAction *actionRead_Maze;
    QAction *actionExit;
    QWidget *centralWidget;
    OpenGLWidget *widget;
    QMenuBar *menuBar;
    QMenu *menuFile;

    void setupUi(QMainWindow *MazeClass)
    {
        if (MazeClass->objectName().isEmpty())
            MazeClass->setObjectName(QStringLiteral("MazeClass"));
        MazeClass->resize(382, 415);
        MazeClass->setDockOptions(QMainWindow::AllowTabbedDocks|QMainWindow::AnimatedDocks);
        actionRead_Maze = new QAction(MazeClass);
        actionRead_Maze->setObjectName(QStringLiteral("actionRead_Maze"));
        actionExit = new QAction(MazeClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        centralWidget = new QWidget(MazeClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        widget = new OpenGLWidget(centralWidget);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(0, 0, 1200, 600));
        widget->setLayoutDirection(Qt::LeftToRight);
        MazeClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MazeClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 382, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        MazeClass->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionRead_Maze);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);

        retranslateUi(MazeClass);

        QMetaObject::connectSlotsByName(MazeClass);
    } // setupUi

    void retranslateUi(QMainWindow *MazeClass)
    {
        MazeClass->setWindowTitle(QApplication::translate("MazeClass", "Project 2 Maze ", 0));
        actionRead_Maze->setText(QApplication::translate("MazeClass", "Read Maze", 0));
        actionExit->setText(QApplication::translate("MazeClass", "Exit", 0));
        menuFile->setTitle(QApplication::translate("MazeClass", "File", 0));
    } // retranslateUi

};

namespace Ui {
    class MazeClass: public Ui_MazeClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAZE_H
