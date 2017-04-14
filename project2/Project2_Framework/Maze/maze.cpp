#include "maze.h"
Maze_Map *Maze::maze;					//Static 一定有一個實體，不然會出現link error
Maze::Maze(QWidget *parent): QMainWindow(parent)
{
	ui.setupUi(this);
	ui.centralWidget->setFixedSize(w,h);
	timer = new QTimer(this);
	//ui.centralWidget->set
	connect(ui.actionRead_Maze,		SIGNAL(triggered()),	this,	SLOT(Read_Maze()));
	connect(ui.actionExit,			SIGNAL(triggered()),	this,	SLOT(Exit()));
	connect(timer,					SIGNAL(timeout()),		this,	SLOT(Refrush_Widget()));

	move_speed = 0.1f;
	move_FB = move_LR = move_Dir = 0;
}
//讀檔案進來
void Maze::Read_Maze()
{
	QDir dir("Maze Map");
	QString temp;
	if(dir.exists())
		temp = "Maze Map/";
	else
		temp = "../x64/Release/Maze Map/";
	filename = QFileDialog::getOpenFileName(this,tr("Read Maze"),temp,tr("Any File (*.*)"));
	maze = new Maze_Map(filename);
	ui.widget->updateGL();
	timer->start(20);
	//maze->ReadFile_byName(filename);
}
void Maze::Refrush_Widget()
{
	if(maze != NULL && !maze->IsEmpty())
	{
		float angle_0 = maze->player.posDir / 180 * 3.14;
		float angle_90 = (maze->player.posDir-90) / 180 * 3.14;
		float inX = move_FB * cos(angle_0) + move_LR * cos(angle_90);
		float inY = move_FB * sin(angle_0) + move_LR * sin(angle_90);
		if(inX != 0 || inY != 0)
			CollisionDetection(inX,inY);
		maze->player.posDir += 30 * move_Dir;
		ui.widget->updateGL();
	}
	else
		timer->stop();
}
void Maze::CollisionDetection(float inx,float iny)
{
	int big_X = maze->edges[maze->edges.size() - 1].end.x;
	int big_Y = maze->edges[maze->edges.size() - 1].end.y;
	float const_dis = 0.01f;									//撞牆前面的誤差值
	bool collision_bool_X = false;
	bool collision_bool_Y = false;
	for (int i = 0 ; i < (int)maze->edges.size() && (!collision_bool_X || !collision_bool_Y); i++)
		if(maze->edges[i].seen)
			switch (Check_Same_X_or_Y(maze->edges[i].start.x,maze->edges[i].start.y,maze->edges[i].end.x,maze->edges[i].end.y,big_X,big_Y))
			{
			case 1:
				if(((maze->player.posX < maze->edges[i].start.x + const_dis && maze->edges[i].start.x - const_dis < maze->player.posX + inx) || (maze->player.posX + inx < maze->edges[i].start.x + const_dis && maze->edges[i].start.x - const_dis < maze->player.posX)) 
					&& maze->edges[i].start.y<= maze->player.posY && maze->edges[i].end.y >= maze->player.posY)
					collision_bool_X = true;
				break;
			case 2:
				if(((maze->player.posY < maze->edges[i].start.y + const_dis && maze->edges[i].start.y - const_dis < maze->player.posY + iny) || (maze->player.posY + iny  < maze->edges[i].start.y + const_dis && maze->edges[i].start.y - const_dis < maze->player.posY ))
					&& maze->edges[i].start.x<= maze->player.posX && maze->edges[i].end.x >= maze->player.posX)
					collision_bool_Y = true;
				break;
			}
	if(!collision_bool_X)
		maze->player.posX += inx;
	if(!collision_bool_Y)
		maze->player.posY += iny;
}
int Maze::Check_Same_X_or_Y(int maze_X1,int maze_Y1,int maze_X2,int maze_Y2,int big_X,int big_Y)
{
	//不考慮斜的邊
	if (maze_X1 == maze_X2)
		return 1;
		/*
		if(maze_X1 == 0 || maze_X1 == big_X)
			return 0;
		else
			return 1;*/
	else if(maze_Y1 == maze_Y2)
		return 2;
		/*
		if(maze_Y1 == 0 || maze_Y1 == big_Y)
			return 0;
		else
			return 2;*/
	else 
		return 3;
}
void Maze::Exit()
{
	QApplication::exit();
}

//Keyboard Down
void Maze::keyPressEvent(QKeyEvent *event)
{
	if(maze != NULL && !maze->IsEmpty())
		switch (event->key())
		{
			//Forward Backward
			case (Qt::Key_W):
				move_FB = move_speed;
				break;
			case (Qt::Key_S):
				move_FB = -move_speed;
				break;

			//Lift Right
			case (Qt::Key_A):
				move_LR = -move_speed;
				break;
			case (Qt::Key_D):
				move_LR = move_speed;
				break;

			case (Qt::Key_Left):
				move_Dir = move_speed;
				break;
			case (Qt::Key_Right):
				move_Dir = -move_speed;
				break;
		}
}
void Maze::keyReleaseEvent(QKeyEvent *event)
{
	if(maze != NULL && !maze->IsEmpty())
		switch (event->key())
		{
			case (Qt::Key_W):
			case (Qt::Key_S):
				move_FB = 0;
				break;

			case (Qt::Key_A):
			case (Qt::Key_D):
				move_LR = 0;
				break;

			case (Qt::Key_Left):
			case (Qt::Key_Right):
				move_Dir = 0;
				break;
		}
}