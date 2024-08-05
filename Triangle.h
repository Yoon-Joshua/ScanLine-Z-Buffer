#pragma once
#include "Vector3D.h"
#include "Light.h"
class Triangle
{
public:
	Vector3D vertices[3];
	Vector3D normals[3];
	Color color;
	Triangle();
};

