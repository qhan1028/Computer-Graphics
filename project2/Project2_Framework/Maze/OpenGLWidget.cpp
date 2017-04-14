#include "OpenGLWidget.h"
#include <iostream>
#include "maze.h"
#include <gl\gl.h>
#include <gl\GLU.h>

#include <conio.h>
#include <windows.h>

#define PI 3.1415926536

int flag = 0;
#define PERIOD 200
#define DEBUG 1
#define DEBUG2 1

OpenGLWidget::OpenGLWidget(QWidget *parent) : QGLWidget(parent)
{
	
	top_z = 1.5f;
	but_z = -1;

	QDir dir("Pic");
	if(dir.exists())
		pic_path = "Pic/";
	else
		pic_path = "../x64/Release/Pic/";
}
void OpenGLWidget::initializeGL()
{
	glClearColor(0,0,0,1);
	glEnable(GL_TEXTURE_2D);
	loadTexture2D(pic_path + "grass.png",grass_ID);
	loadTexture2D(pic_path + "sky.png",sky_ID);
}
void OpenGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(Maze::maze!=NULL && !Maze::maze->IsEmpty())
	{
		//View 1
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0 , 0 , Maze::w/2 , Maze::h);
		glOrtho (-0.1, Maze::maze->Max_WH +0.1, -0.1 , Maze::maze->Max_WH +0.1, 0 , 10);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Mini_Map();
		
		//View 2
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(Maze::w/2,0,Maze::w/2,Maze::h);
		gluPerspective(Maze::maze->player.posFov , 1 , 0.01 , 200);
		
		/* gluLookAt
		//原本相機位置
		//看的方向
		//哪邊是上面
		*/
		gluLookAt(Maze::maze->player.posX,Maze::maze->player.posZ,Maze::maze->player.posY , 
			Maze::maze->player.posX + cos(degree_change(Maze::maze->player.posDir)),Maze::maze->player.posZ, Maze::maze->player.posY + sin(degree_change(Maze::maze->player.posDir)),
			0.0, -1.0, 0.0);
		Map_3D();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//Map_3D();
	}
}
void OpenGLWidget::resizeGL(int w,int h)
{
}

// Draw Left Part
void OpenGLWidget::Mini_Map()	
{
	glBegin(GL_LINES);
		for(int i = 0 ; i < (int)Maze::maze->edges.size() ; i++)
		{
			glColor3f(Maze::maze->edges[i].r , Maze::maze->edges[i].g , Maze::maze->edges[i].b);
			if(Maze::maze->edges[i].seen)
			{
				glVertex2f(Maze::maze->edges[i].start.x , Maze::maze->edges[i].start.y);
				glVertex2f(Maze::maze->edges[i].end.x , Maze::maze->edges[i].end.y);
			}
		}

		float len = 25;
		glColor3f(1,1,1);
		glVertex2f(Maze::maze->player.posX , Maze::maze->player.posY );
		glVertex2f(Maze::maze->player.posX + len/Maze::maze->Max_WH * cos(degree_change(Maze::maze->player.posDir-Maze::maze->player.posFov/2)) ,
			Maze::maze->player.posY + len/Maze::maze->Max_WH * sin(degree_change(Maze::maze->player.posDir-Maze::maze->player.posFov/2)));

		glVertex2f(Maze::maze->player.posX , Maze::maze->player.posY );
		glVertex2f(Maze::maze->player.posX + len/Maze::maze->Max_WH * cos(degree_change(Maze::maze->player.posDir+Maze::maze->player.posFov/2)) ,
			Maze::maze->player.posY + len/Maze::maze->Max_WH * sin(degree_change(Maze::maze->player.posDir+Maze::maze->player.posFov/2)));
	glEnd();
}

// Draw Right Part
void printTab(int n)
{
	for (int i = 0; i < n; i++)
	{
		printf("    ");
	}
}

void worldToView(Edge_Info &edge)
{
	double player_angle = Maze::maze->player.posDir;
	double a = -1 * (-90 + player_angle) * PI / 180;
	double px = -1 * Maze::maze->player.posX;
	double py = -1 * Maze::maze->player.posY;
	double pz = -1 * Maze::maze->player.posZ;

	// combine rotate and tranlate matrix
	double rotate[4][4] = {
		{cos(a), 0, -1 * sin(a), 0},
		{     0, 1,           1, 0},
		{sin(a), 0,      cos(a), 0},
		{     0, 0,           0, 1}
	};
	double translate[4][4] = {
		{1, 0, 0, -1 * px},
		{0, 1, 0, -1 * pz},
		{0, 0, 1, -1 * py},
		{0, 0, 0,       1}
	};
	double rt[4][4] = {
		{cos(a), 0, -1 * sin(a), cos(a) * px - sin(a) * py},
		{     0, 1,           0,                        pz},
		{sin(a), 0,      cos(a), sin(a) * px + cos(a) * py},
		{     0, 0,           0,                         1}
	};

	// translate and rotate edge
	double sp[4] = {edge.start.x, 0, edge.start.y, 1}; // start of edge
	double ep[4] = {edge.end.x, 0, edge.end.y, 1}; // end of edge
	double sx = 0, sy = 0, ex = 0, ey = 0; // result start point: (sx, sy)
	for (int j = 0; j < 4; j++)
	{
		sx += rt[0][j] * sp[j];
		sy += rt[2][j] * sp[j];
		ex += rt[0][j] * ep[j];
		ey += rt[2][j] * ep[j];
	}
	edge.start.x = sx;
	edge.start.y = sy;
	edge.end.x = ex;
	edge.end.y = ey;	
}

struct Point{
	double x;
	double y;

	Point(): x(0), y(0) {}
	Point(double x, double y): x(x), y(y) {}
};

struct Wall_Point{
	vector<Point> points;
	bool exist;
	float r, g, b;

	Wall_Point() : r(0), g(0), b(0), exist(false) {}
	Wall_Point(float r, float g, float b) : r(r), g(g), b(b) {}
};

int inSight(double a, double b, double x, double y)
{
	// y = ax + b
	double in_sight = y - a * x - b;
	if (in_sight >= 0)
		return 1;
	else
		return -1;
}

int inAngle(double left_fov, double right_fov, double angle)
{
	if (angle >= right_fov && angle <= left_fov) return 1;
	else return -1;
}

void findPoint(Point &p, double a1, double b1, double c1, double a2, double b2, double c2)
{
	// ax + by = c
	double delta = a1 * b2 - a2 * b1;
	double delta_x = c1 * b2 - c2 * b1;
	double delta_y = a1 * c2 - a2 * c1;
	p = Point(delta_x / delta, delta_y / delta);
}

void findIntersection(Point &result, double sx, double sy, double ex, double ey, double lf_a)
{
	if (abs(sx - ex) >= 0.0001) {
		double fov_a = (sy - ey) / (sx - ex); // y = "a"x + b
		double fov_c = (sx * ey - ex * sy) / (sx - ex); // y = ax + "b"
		findPoint(result, -1*lf_a, 1, 0, -1*fov_a, 1, fov_c); // point, a1, b1, c1, a2, b2, c2
	} else { // slope too large
		findPoint(result, -1*lf_a, 1, 0, 1, 0, sx); // point, a1, b1, c1, a2, b2, c2
	}
}

double findAngle(double x, double y) // return with degree (-90 ~ 270)
{
	double degrees;
	if (x > 0 && y >= 0)
	{
		double radians = atan(y / x);
		degrees = radians * 180 / PI;		
	}
	else if (x < 0 && y >= 0)
	{
		double radians = atan(y / abs(x));
		degrees = 180 - radians * 180 / PI;
	}
	else if (x > 0 && y <= 0)
	{
		double radians = atan(abs(y) / x);
		degrees = 0 - radians * 180 / PI;
	}
	else if (x < 0 && y <= 0)
	{
		double radians = atan(abs(y) / abs(x));
		degrees = 180 + radians * 180 / PI;
	}
	else if (x == 0) 
	{
		if (y >= 0) degrees = 90;
		else degrees = -90;
	}
	return degrees;
}

void viewToSight(Wall_Point &wp, Edge_Info &edge, double lf, double rf, int edge_no, int depth)
{
	if (DEBUG && flag % PERIOD == 0) printf("edge %d, fov %f to %f: ", edge_no, rf, lf);
	
	// set color
	wp = Wall_Point(edge.r, edge.g, edge.b);
	wp.exist = false;

	// fov slope
	double lf_a = tan(lf * PI / 180); // left fov slope
	double rf_a = tan(rf * PI / 180); // right fov slope
	// edge points
	double sx = edge.start.x;
	double sy = edge.start.y;
	double ex = edge.end.x;
	double ey = edge.end.y;
	// intermediate value theorem
	double lf_s = inSight(lf_a, 0, sx, sy); // 1: in-sight, -1: out-of-sight
	double lf_e = inSight(lf_a, 0, ex, ey);
	double rf_s = inSight(rf_a, 0, sx, sy);
	double rf_e = inSight(rf_a, 0, ex, ey);
	// find angle by arctan
	double angle_s = findAngle(sx, sy);
	double angle_e = findAngle(ex, ey);
	if (DEBUG && flag % PERIOD == 0) printf("s: %f, e: %f, ", angle_s, angle_e);
	
	if (lf_s * lf_e > 0 && rf_s * rf_e > 0) // case 0: no intersection on left and right fov
	{
		if (sy > 0 && ey > 0)
		{
			if (inAngle(lf, rf, angle_s) > 0 && inAngle(lf, rf, angle_e) > 0) // in sight
			{
				wp.exist = true;
				wp.points.push_back(Point(sx, sy));
				wp.points.push_back(Point(ex, ey));
				if (DEBUG && flag % PERIOD == 0) printf("p at case: 0-1\n"); // "p"aint at case: 0-1 
			}
			else if ((angle_s > lf && angle_e < rf) || (angle_e > lf && angle_s < rf)) // covered sight
			{
				wp.exist = true;
				wp.points.push_back(Point(sx, sy));
				wp.points.push_back(Point(ex, ey));
				if (DEBUG && flag % PERIOD == 0) printf("p at case: 0-2\n");
			}
			else
			{
				if (DEBUG && flag % PERIOD == 0) printf("np at case: 0-3\n"); // "n"ot "p"aint at case 0-3
			}
		}
		else // out of sight
		{
			if (DEBUG && flag % PERIOD == 0) printf("np at case: 0-4\n");
		}
	}
	else if (lf_s * lf_e < 0 && rf_s * rf_e > 0) // case left: intersection only on left
	{
		Point p; // intersection point
		findIntersection(p, sx, sy, ex, ey, lf_a);
		
		if (p.y < 0) // not in sight
		{
			if (DEBUG && flag % PERIOD == 0) printf("np at case: left-1\n");
		}
		else if (p.y >= 0 && inAngle(lf, rf, angle_e) > 0 && inAngle(lf, rf, angle_s) > 0) // from start to end pos
		{
			wp.exist = true;
			wp.points.push_back(Point(sx, sy));
			wp.points.push_back(Point(ex, ey));
			if (DEBUG && flag % PERIOD == 0) printf("p at case: left-0.5\n");
		}
		else if (p.y >= 0 && inAngle(lf, rf, angle_s) > 0) // from p to start pos
		{
			wp.exist = true;
			wp.points.push_back(Point(p.x, p.y));
			wp.points.push_back(Point(sx, sy));
			if (DEBUG && flag % PERIOD == 0) printf("p at case: left-2\n");
		}
		else if (p.y >= 0 && inAngle(lf, rf, angle_e) > 0) // from p to end pos
		{
			wp.exist = true;
			wp.points.push_back(Point(p.x, p.y));
			wp.points.push_back(Point(ex, ey));
			if (DEBUG && flag % PERIOD == 0) printf("p at case: left-3\n");
		}
		else {
			int si = inAngle(lf, rf, angle_s);
			int ei = inAngle(lf, rf, angle_e);
			if (DEBUG && flag % PERIOD == 0) printf("left-error, p.y = %f, in(s) = %d, in(e) = %d\n", p.y, si, ei);
		}
	}
	else if (lf_s * lf_e > 0 && rf_s * rf_e < 0) // case right: intersection only on right
	{
		// find intersection
		Point p; // intersection point
		findIntersection(p, sx, sy, ex, ey, rf_a);

		if (p.y < 0) // not in sight
		{
			if (DEBUG && flag % PERIOD == 0) printf("np at case: right-0\n");
		}
		else if (p.y >= 0 && inAngle(lf, rf, angle_e) > 0 && inAngle(lf, rf, angle_s) > 0) // from start to end pos
		{
			wp.exist = true;
			wp.points.push_back(Point(sx, sy));
			wp.points.push_back(Point(ex, ey));
			if (DEBUG && flag % PERIOD == 0) printf("p at case: right-0.5\n");
		}
		else if (p.y >= 0 && inAngle(lf, rf, angle_s) > 0) // from p to start pos
		{
			wp.exist = true;
			wp.points.push_back(Point(p.x, p.y));
			wp.points.push_back(Point(sx, sy));
			if (DEBUG && flag % PERIOD == 0) printf("p at case: right-1\n");
		}
		else if (p.y >= 0 && inAngle(lf, rf, angle_e) > 0) // from p to end pos
		{
			wp.exist = true;
			wp.points.push_back(Point(p.x, p.y));
			wp.points.push_back(Point(ex, ey));
			if (DEBUG && flag % PERIOD == 0) printf("p at case: right-2\n");
		}
		else {
			int si = inAngle(lf, rf, angle_s);
			int ei = inAngle(lf, rf, angle_e);
			if (DEBUG && flag % PERIOD == 0) printf("right-error, p.y = %f, in(s) = %d, in(e) = %d\n", p.y, si, ei);
		}
	}
	else if (lf_s * lf_e < 0 && rf_s * rf_e < 0) // case 2: intersection on right and left
	{
		Point pl, pr; // intersection point on left and right fov
		findIntersection(pl, sx, sy, ex, ey, lf_a);
		findIntersection(pr, sx, sy, ex, ey, rf_a);
		
		if (pl.y < 0 && pr.y < 0) // not in sight
		{
			if (DEBUG && flag % PERIOD == 0) printf("np at case: 2-0\n");
		}
		else if (pl.y >= 0 && pr.y < 0) // left
		{
			wp.exist = true;
			wp.points.push_back(Point(pl.x, pl.y));
			wp.points.push_back((lf_s > 0)? Point(sx, sy) : Point(ex, ey));
			if (DEBUG && flag % PERIOD == 0) printf("p at case: 2-1\n");
		}
		else if (pl.y < 0 && pr.y >= 0) // right
		{
			wp.exist = true;
			wp.points.push_back(Point(pr.x, pr.y));
			wp.points.push_back((rf_s > 0)? Point(sx, sy) : Point(ex, ey));
			if (DEBUG && flag % PERIOD == 0) printf("p at case: 2-2\n");
		}
		else if (pl.y >= 0 && pr.y >= 0) // left and right
		{
			wp.exist = true;
			wp.points.push_back(Point(pl.x, pl.y));
			wp.points.push_back(Point(pr.x, pr.y));
			if (DEBUG && flag % PERIOD == 0) printf("p at case: 2-3\n");
		}
		else {
			int si = inAngle(lf, rf, angle_s);
			int ei = inAngle(lf, rf, angle_e);
			if (DEBUG && flag % PERIOD == 0) printf("2-error, pl.y = %f, pr.y = %f, in(s) = %d, in(e) = %d\n", pl.y, pr.y, si, ei);
		}
	}
	else {
		if (DEBUG && flag % PERIOD == 0) printf("np at case error\n");
	}
}

void sightTo2D(Wall_Point &wp)
{
	// map x to [-1, 1], y to [-1, 1]
	double angle = (Maze::maze->player.posFov / 2) * PI / 180;
	double t = tan(angle);
	
	if (wp.exist)
	{
		for (int i = 0; i < (int)wp.points.size(); i++)
		{
			double x = wp.points[i].x;
			double y = wp.points[i].y;
			wp.points[i].x = x / (y * t);
			wp.points[i].y = 1 / (y * t);
		}
	}
}

void draw2D(Wall_Point &wp)
{
	if (wp.exist) // exist: in sight?
	{
		glBegin(GL_POLYGON);
			glColor3f(wp.r, wp.g, wp.b);
			glVertex2f(wp.points[0].x, wp.points[0].y);
			glVertex2f(wp.points[0].x, -1 * wp.points[0].y);
			glVertex2f(wp.points[1].x, -1 * wp.points[1].y);
			glVertex2f(wp.points[1].x, wp.points[1].y);
		glEnd();
	}
}

bool inCell(Cell_Info cell, double x, double y, double d)
{
	if (abs(cell.center_x - x) <= d/2 && abs(cell.center_y - y) <= d/2)
		return true;
	else 
		return false;
}

int findCell(double x, double y, double d)
{
	vector<Cell_Info> cells = Maze::maze->cells;
	for (int i = 0; i < (int)cells.size(); i++) 
	{
		if (inCell(cells[i], x, y, d)) return i;
	}
	return -1;
}

void drawWall(Edge_Info edge, double lf, double rf, int edge_no, int depth)
{
	Wall_Point wp;
	worldToView(edge);
	viewToSight(wp, edge, lf, rf, edge_no, depth);
	sightTo2D(wp);
	draw2D(wp);
}

int findNeighborCell(int curr_cell, int edge)
{
	vector<Cell_Info> cells = Maze::maze->cells;
	for (int i = 0; i < (int)cells.size(); i++)
	{
		if (i == curr_cell) continue;

		for (int j = 0; j < 4; j++) // four edges
		{
			if (cells[i].e[j] == edge) return i;
		}
	}
	return -1;
}

void splitSight(Edge_Info &edge, double lf, double rf, double &new_lf, double &new_rf, int depth)
{
	worldToView(edge);

	double sx = edge.start.x;
	double sy = edge.start.y;
	double ex = edge.end.x;
	double ey = edge.end.y;
	double angle_s = findAngle(sx, sy);
	double angle_e = findAngle(ex, ey);
	
	if (inAngle(lf, rf, angle_s) > 0 && inAngle(lf, rf, angle_e) > 0) {
		if (angle_s >= angle_e)
		{
			new_lf = angle_s;
			new_rf = angle_e;
			if ((DEBUG2) && flag % PERIOD == 0) {
				printTab(depth);
				printf("in case 1-1: s: %f, e: %f\n", angle_s, angle_e);
			}
			if ((DEBUG2) && flag % PERIOD == 0 && new_rf == new_lf) {
				printTab(depth);
				printf("error angle in case 1-1\n");
			}
		}
		else
		{
			new_lf = angle_e;
			new_rf = angle_s;
			if ((DEBUG2) && flag % PERIOD == 0) {
				printTab(depth);
				printf("in case 1-2: s: %f, e: %f\n", angle_s, angle_e);
			}
			if ((DEBUG2) && flag % PERIOD == 0 && new_rf == new_lf) {
				printTab(depth);
				printf("error angle in case 1-2\n");
			}
		}
	}
	else if (inAngle(lf, rf, angle_s) > 0) // edge start point in angle
	{
		if (depth == 0) { // in first cell, can't use angle
			double X = (abs(sy) * ex + abs(ey) * sx) / (abs(sy) + abs(ey));
			if (X < 0)
			{
				new_lf = lf;
				new_rf = angle_s;
				if ((DEBUG2) && flag % PERIOD == 0) {
					printTab(depth);
					printf("in case 2-1: s: %f, e: %f\n", angle_s, angle_e);
				}
				if ((DEBUG2) && flag % PERIOD == 0 && new_rf == new_lf) {
					printTab(depth);
					printf("error angle in case 2-1\n");
				}
			}
			else
			{
				new_lf = angle_s;
				new_rf = rf;
				if ((DEBUG2) && flag % PERIOD == 0) {
					printTab(depth);
					printf("in case 2-2: s: %f, e: %f\n", angle_s, angle_e);
				}
				if ((DEBUG2) && flag % PERIOD == 0 && new_rf == new_lf) {
					printTab(depth);
					printf("error angle in case 2-2\n");
				}
			}
		} else { // in recursion, can't use X, thus use angle
			if (angle_e >= lf) {
				new_lf = lf;
				new_rf = angle_s;
				if ((DEBUG2) && flag % PERIOD == 0) {
					printTab(depth);
					printf("in case 2r-1: s: %f, e: %f\n", angle_s, angle_e);
				}
				if ((DEBUG2) && flag % PERIOD == 0 && new_rf == new_lf) {
					printTab(depth);
					printf("error angle in case 2-1\n");
				}
			} else {
				new_lf = angle_s;
				new_rf = rf;
				if ((DEBUG2) && flag % PERIOD == 0) {
					printTab(depth);
					printf("in case 2r-2: s: %f, e: %f\n", angle_s, angle_e);
				}
				if ((DEBUG2) && flag % PERIOD == 0 && new_rf == new_lf) {
					printTab(depth);
					printf("error angle in case 2-2\n");
				}
			}
		}
	}
	else if (inAngle(lf, rf, angle_e) > 0) // edge end in angle
	{
		if (depth == 0) { // in first cell, can't use angle
			double X = (abs(sy) * ex + abs(ey) * sx) / (abs(sy) + abs(ey));
			if (X < 0) 
			{
				new_lf = lf;
				new_rf = angle_e;
				if ((DEBUG2) && flag % PERIOD == 0) {
					printTab(depth);
					printf("in case 3-1: s: %f, e: %f\n", angle_s, angle_e);
				}
				if ((DEBUG2) && flag % PERIOD == 0 && new_rf == new_lf) {
					printTab(depth);
					printf("error angle in case 3-1\n");
				}
			}
			else
			{
				new_lf = angle_e;
				new_rf = rf;
				if ((DEBUG2) && flag % PERIOD == 0) {
					printTab(depth);
					printf("in case 3-2: s: %f, e: %f\n", angle_s, angle_e);
				}
				if ((DEBUG2) && flag % PERIOD == 0 && new_rf == new_lf) {
					printTab(depth);
					printf("error angle in case 3-2\n");
				}
			}
		} else { // in recursion, can't use X, thus use angle
			if (angle_s >= lf) {
				new_lf = lf;
				new_rf = angle_e;
				if ((DEBUG2) && flag % PERIOD == 0) {
					printTab(depth);
					printf("in case 3r-1: s: %f, e: %f\n", angle_s, angle_e);
				}
				if ((DEBUG2) && flag % PERIOD == 0 && new_rf == new_lf) {
					printTab(depth);
					printf("error angle in case 3-1\n");
				}
			} else {
				new_lf = angle_e;
				new_rf = rf;
				if ((DEBUG2) && flag % PERIOD == 0) {
					printTab(depth);
					printf("in case 3r-2: s: %f, e: %f\n", angle_s, angle_e);
				}
				if ((DEBUG2) && flag % PERIOD == 0 && new_rf == new_lf) {
					printTab(depth);
					printf("error angle in case 3-2\n");
				}
			}
		}
	}
	else if (inAngle(lf, rf, angle_e) < 0 && inAngle(lf, rf, angle_s) < 0) // both not in sight, can't use angle.
	{
		double Y = (abs(sx) * ey + abs(ex) * sy) / (abs(sx) + abs(ex));
		if (Y > 0 && ((angle_e > lf && angle_s < rf) || (angle_s > lf && angle_e < rf) ))
		{
			new_lf = lf;
			new_rf = rf;
		}
		else
		{
			new_rf = new_lf = 0;
		}
	}
	else
	{
		new_lf = 0;
		new_rf = 0;
		if ((DEBUG2) && flag % PERIOD == 0) {
			printTab(depth);
			printf("in case 5: s: %f, e: %f\n", angle_s, angle_e);
		}
		if ((DEBUG2) && flag % PERIOD == 0 && new_rf == new_lf) {
			printTab(depth);
			printf("error angle in case 5\n");
		}
	}
	if (new_lf < new_rf) {
		double tmp = new_rf;
		new_rf = new_lf;
		new_lf = tmp;
	}
}

void drawCell(int curr_cell, double lf, double rf, int prev_edge, int depth)
{
	vector<Edge_Info> all_edges = Maze::maze->edges;
	vector<Cell_Info> all_cells = Maze::maze->cells;
	Cell_Info cell = all_cells[curr_cell];
	if ((DEBUG || DEBUG2) && flag % PERIOD == 0) {
		printTab(depth);
		printf("draw cell %d in: %f to %f\n", curr_cell, rf, lf);
	}
	for (int i = 0; i < 4; i++)
	{	
		Edge_Info edge = all_edges[cell.e[i]];

		if (edge.seen)
		{
			//if ((DEBUG2) && flag % PERIOD == 0) printf("edge %d seen\n", cell.e[i]);
			if (DEBUG2 && flag % PERIOD == 0) {
				printTab(depth);
				printf("cell %d, ", curr_cell);
			}
			drawWall(edge, lf, rf, cell.e[i], depth);
		}
		else // transparent edge
		{
			if ((DEBUG2) && flag % PERIOD == 0) {
				printTab(depth);
				printf("cell %d, edge %d not seen\n", curr_cell, cell.e[i]);
			}
			if (cell.e[i] == prev_edge) continue;
			// split sight
			double new_lf, new_rf;
			int next_cell = findNeighborCell(curr_cell, cell.e[i]);
			if ((DEBUG2) && flag % PERIOD == 0) {
				printTab(depth);
				printf("cell %d, before recursion to %d, old fov = %f to %f\n", curr_cell, next_cell, rf, lf);
			}
			splitSight(edge, lf, rf, new_lf, new_rf, depth);
			// draw next cell
			if ((DEBUG2) && flag % PERIOD == 0) {
				printTab(depth);
				printf("cell %d, before recursion to %d, new fov = %f to %f\n", curr_cell, next_cell, new_rf, new_lf);
			}
			if (new_lf - new_rf > 0) {
				if ((DEBUG2) && flag % PERIOD == 0) {
					printTab(depth);
					printf("in recursion to %d\n", next_cell);
				}
				drawCell(next_cell, new_lf, new_rf, cell.e[i], depth+1);
			}
		}
	}
}

void OpenGLWidget::Map_3D()
{
	glLoadIdentity();

	// 畫右邊區塊的所有東西
	glBindTexture(GL_TEXTURE_2D, sky_ID);

	double cell_size = abs(Maze::maze->edge_length);
	int curr_cell = findCell(Maze::maze->player.posX, Maze::maze->player.posY, cell_size);
	if ((DEBUG || DEBUG2) && flag % PERIOD == 0) printf("===== start at cell %d\n", curr_cell);

	double left_fov = (90 + Maze::maze->player.posFov/2);
	double right_fov = (90 - Maze::maze->player.posFov/2);
	drawCell(curr_cell, left_fov, right_fov, -1, 0);

	flag += 1;
	
	glDisable(GL_TEXTURE_2D);
}
// end draw right part

void OpenGLWidget::loadTexture2D(QString str,GLuint &textureID)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
	QImage img(str);
	QImage opengl_grass = QGLWidget::convertToGLFormat(img);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, opengl_grass.width(), opengl_grass.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl_grass.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_2D);
}
float OpenGLWidget::degree_change(float num)
{
	return num /180 * 3.14159f;
}