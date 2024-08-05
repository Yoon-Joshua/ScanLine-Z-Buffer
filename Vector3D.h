#pragma once
class Vector3D
{
public:
	double x, y, z;

	Vector3D(double, double, double);

	Vector3D() {}

	Vector3D operator+(Vector3D);

	Vector3D operator+(double);

	Vector3D operator-(Vector3D);

	Vector3D operator*(double);

	Vector3D operator/(double);

	Vector3D normalize();

	static Vector3D crossProduct(Vector3D&, Vector3D&);
	static double dot(Vector3D&, Vector3D&);
};

