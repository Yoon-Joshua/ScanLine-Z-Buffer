#include "Vector3D.h"
#include<cmath>

Vector3D::Vector3D(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3D Vector3D::operator+(Vector3D v) {
	double x_new = x + v.x;
	double y_new = y + v.y;
	double z_new = z + v.z;
	return Vector3D(x_new, y_new, z_new);
}

Vector3D Vector3D::operator-(Vector3D v) {
	double x_new = x - v.x;
	double y_new = y - v.y;
	double z_new = z - v.z;
	return Vector3D(x_new, y_new, z_new);
}

Vector3D Vector3D::operator*(double v) {
	double x_new = x * v;
	double y_new = y * v;
	double z_new = z * v;
	return Vector3D(x_new, y_new, z_new);
}

Vector3D Vector3D::crossProduct(Vector3D& a, Vector3D& b) {
	Vector3D ret;
	ret.x = a.y*b.z - a.z*b.y;
	ret.y = a.z*b.x - a.x*b.z;
	ret.z = a.x*b.y - a.y*b.x;
	return ret;
}

Vector3D Vector3D::operator+(double v) {
	Vector3D ret;
	ret.x = x + v;
	ret.y = y + v;
	ret.z = z + v;
	return ret;
}

Vector3D Vector3D::operator/(double v) {
	Vector3D ret;
	ret.x = x / v;
	ret.y = y / v;
	ret.z = z / v;
	return ret;
}

double Vector3D::dot(Vector3D& a, Vector3D& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3D Vector3D::normalize() {
	double length = sqrt(x * x + y * y + z * z);
	Vector3D ret = *this;
	if (length < 1e-10) {
		return Vector3D(0, 0, 0);
	}
	else {
		ret = ret / length;
		return ret;
	}
}