#ifndef MAZE_H
#define MAZE_H

#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <Maze_map.h>
#include <QKeyEvent>
#include <QTimer>
#include <QDir>
#include "ui_maze.h"

class Maze : public QMainWindow
{
	Q_OBJECT
public:
	Maze(QWidget *parent = 0);
	static Maze_Map *maze;
	static const int w = 1200;
	static const int h = 600;
private:
	Ui::MazeClass ui;

	QString filename;
	QTimer *timer;
	void keyPressEvent(QKeyEvent *);
	void keyReleaseEvent(QKeyEvent *);
	
	float move_speed;
	//LB 前面後面 LR左右
	float move_FB,move_LR, move_Dir;
private slots:
	void Read_Maze();
	void Refrush_Widget();

	void CollisionDetection(float,float);
	int Check_Same_X_or_Y(int,int,int,int,int,int);		//0代表是邊界，1代表同X，2代表同Y，3代表斜線
	void Exit();
};

#endif // MAZE_H
