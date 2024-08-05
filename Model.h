#pragma once
#include "Vector3D.h"
#include "Triangle.h"
#include <vector>
#include <string>
#include "Light.h"

class Model {
private:
	typedef struct Face {
		int vertexIndex[3];
		int normalIndex[3];
		int textureIndex[3];
		Color color;
	} Face;
	std::vector<Vector3D> vertices;
	std::vector<Face> faces;
	std::vector<Vector3D> normals;

public:
	double xmin, xmax, ymin, ymax, zmin, zmax;
	double min, span;
	void load(std::string);
	size_t getVerticesNum();
	size_t getFacesNum();
	Triangle getTriangle(int);
	void clear();
	void reverseXY();
	void reverseYZ();
	void reverseZX();
	void shined(Light&);
};