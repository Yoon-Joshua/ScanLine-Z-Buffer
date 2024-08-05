#pragma once
#include "Vector3D.h"

struct Color {
	unsigned char red;
	unsigned char blue;
	unsigned char green;
};

class Light
{
private:
	int type;	//光照类型	0-平行光
	Vector3D direction;
	unsigned char color[3];
public:
	Light();
	void setType(int);
	void setDirection(Vector3D);
	void setColor(unsigned char, unsigned char, unsigned char);
	Vector3D getDirection();
	Color getColor();
};

