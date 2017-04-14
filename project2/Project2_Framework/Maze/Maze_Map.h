#pragma once
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

#include <iostream>
#include <vector>
using namespace std;
struct Vertex_Info
{
	double x;
	double y;
};
struct Edge_Info{
	Vertex_Info start;
	Vertex_Info end;
	bool seen;
	float r,g,b;
};
struct Cell_Info{
	vector<int> e;
	double center_x;
	double center_y;
};
struct Player_Info{
	double posX,posY,posZ,posDir,posFov;
};
class Maze_Map
{
public:
	Maze_Map(QString);
	void Test_Name(QString);
	bool IsEmpty();

	int Max_WH;
	int num_vertex,num_edge,num_cell;
	double edge_length;
	Player_Info player;
	vector<Vertex_Info> vertices;
	vector<Edge_Info> edges;
	vector<Cell_Info> cells;
private:	
	
	QMessageBox msgBox;
};

