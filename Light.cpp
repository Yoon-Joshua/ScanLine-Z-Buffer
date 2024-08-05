#include "Light.h"
#include <cmath>

Light::Light() {
	setType(0);
	setColor(255, 255, 255);
	setDirection(Vector3D(0, 0, 1));
}

void Light::setType(int t) {
	type = t;
}

void Light::setColor(unsigned char r, unsigned char g, unsigned char b) {
	color[0] = r;
	color[1] = g;
	color[2] = b;
}

void Light::setDirection(Vector3D d) {
	direction = d;
	double length = sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
	direction = direction / length;
}

Color Light::getColor() {
	Color ret;
	ret.red = color[0];
	ret.green = color[1];
	ret.blue = color[2];
	return ret;
}

Vector3D Light::getDirection() {
	return this->direction;
}