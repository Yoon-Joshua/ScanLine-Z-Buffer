#pragma once

#include "Model.h"
#include "Vector3D.h"
#include "Triangle.h"
#include <list>
#include<opencv2/opencv.hpp>

#define XCOORDINATE_SPAN 2.0;
#define YCOORDINATE_SPAN 2.0;
#define ZCOORDINATE_SPAN 2.0;

typedef struct Edge {
	double ymax;
	double ymin;

	// 与当前扫描线交点的x坐标
	double x;

	double increment;
	unsigned polygonID;
}Edge;

/*
@brief Plane equation in form of Ax+By+Cz+D=0;
*/
typedef struct Plane {
	double a, b, c, d;
}Plane;

/*
@brief 
*/
typedef struct Polygon {
	Plane plane;
	double ymax;
	double ymin;
	std::vector<int> edges;
	unsigned id;
	Color color;
}Polygon;

class Z_buffer
{
private:
	int width;
	int height;

	double *depthBuffer;
	unsigned char *colorBuffer;
	unsigned char *frame;

	Vector3D background;

	std::vector<Edge> all_edges;
	std::vector<Polygon> all_polygons;
	std::vector<int> *polygons;
	std::vector<int> *edges;
	std::vector<int> active_polygons;
	std::vector<int> active_edges;

	Plane calculatePlaneEquation(Vector3D&, Vector3D&, Vector3D&);
public:
	Z_buffer(int, int);
	~Z_buffer();
	void loadMesh(Model&, double);
	void setBackground(Vector3D);
	void scan();
	unsigned char* getFrame();
	void clear();
};

