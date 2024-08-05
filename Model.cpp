/**
* @file Model.cpp
* @brief Model类用来读取.obj文件，加载网格模型
* @author 尹喜润
*/

#include "Model.h"
#include <fstream>
#include <iostream>
#include <sstream>

size_t Model::getFacesNum() { return faces.size(); }
size_t Model::getVerticesNum() { return vertices.size(); }

/**
* @brief	清除模型所有数据
*/
void Model::clear() {
	faces.clear();
	vertices.clear();
	normals.clear();
}

/**
* @brief	加载模型
* @param	filepath	文件路径
*/
void Model::load(std::string filepath) {

	xmin = DBL_MAX;
	xmax = DBL_MIN;
	ymin = DBL_MAX;
	ymax = DBL_MIN;
	zmin = DBL_MAX;
	zmax = DBL_MIN;

	this->clear();

	std::ifstream f(filepath);
	int i = 0;
	while (!f.eof()) {
		std::string s;
		std::getline(f, s);
		std::stringstream ss(s);

		std::string flag;
		ss >> flag;
		if (flag == "#") continue;
		else if (flag == "v") {
			double x, y, z;
			ss >> x;
			ss >> y;
			ss >> z;
			this->vertices.emplace_back(Vector3D(x, y, z));
			if (x < xmin) xmin = x;
			if (x > xmax) xmax = x;
			if (y < ymin) ymin = y;
			if (y > ymax) ymax = y;
			if (z > zmax) zmax = z;
			if (z < zmin) zmin = z;
		}
		else if (flag == "vn") {
			double x, y, z;
			ss >> x;
			ss >> y;
			ss >> z;
			this->normals.emplace_back(Vector3D(x, y, z));
		}
		else if (flag == "f") {
			std::string temp;
			Face face;
			face.color.red = 255;
			face.color.green = 255;
			face.color.blue = 255;
			for (int i = 0; i < 3; i++) {
				ss >> temp;
				std::string indexStr = "";
				std::vector<int> index;
				for (int j = 0; j <= temp.length(); j++) {
					if (temp[j] == '/' || j == temp.length()) {
						if (indexStr.size() == 0) index.emplace_back(-1);
						else index.emplace_back(atoi(indexStr.c_str()));
						indexStr = "";
					}
					else
						indexStr = indexStr + temp[j];
				}
				if (index.size() == 1) {
					face.vertexIndex[i] = index[0] - 1;
					face.normalIndex[i] = -1;
					face.normalIndex[i] = -1;
				}
				else if (index.size() == 3) {
					face.vertexIndex[i] = index[0] - 1;
					face.textureIndex[i] = index[1] - 1;
					face.normalIndex[i] = index[2] - 1;
				}
				else std::cout << "FAIL TO LOAD MODEL" << std::endl;
			}
			faces.emplace_back(face);
		}
	}
	f.close();
	std::cout << "Amount of vertices is " << vertices.size() << std::endl;
	std::cout << "Amount of faces is " << faces.size() << std::endl;
	std::cout << "Amount of normals is " << normals.size() << std::endl;
	std::cout << "xmin = " << xmin << "  xmax = " << xmax << std::endl;
	std::cout << "ymin = " << ymin << "  ymax = " << ymax << std::endl;
	std::cout << "zmin = " << zmin << "  zmax = " << zmax << std::endl;
	
	span = DBL_MIN;
	if (ymax - ymin > span) span = ymax - ymin;
	if (xmax - xmin > span) span = xmax - xmin;
	if (zmax - zmin > span) span = zmax - zmin;
	span *= sqrt(2);

	Vector3D centre;
	centre.x = (xmin + xmax) / 2;
	centre.y = (ymin + ymax) / 2;
	centre.z = (zmin + zmax) / 2;
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].x -= centre.x;
		vertices[i].y -= centre.y;
		vertices[i].z -= centre.z;
	}
}

/**
* @brief	返回指定索引的三角形
* @param	index	三角形的索引
* @return	返回三角形
**/
Triangle Model::getTriangle(int index) {
	Triangle ret;
	for (int i = 0; i < 3; i++) {
		int a, b;
		a = faces[index].vertexIndex[i];
		b = faces[index].normalIndex[i];
		if (a >= 0 && a < vertices.size())
			ret.vertices[i] = vertices[a];
		else
			std::cout << index << "!!!!!!!" << std::endl;
	}
	ret.color = faces[index].color;
	return ret;
}

/**
* @brief	翻转X、Y坐标轴
**/
void Model::reverseXY() {
	for (int i = 0; i < vertices.size(); i++) {
		double temp = vertices[i].x;
		vertices[i].x = vertices[i].y;
		vertices[i].y = temp;
	}
}

void Model::reverseYZ() {
	for (int i = 0; i < vertices.size(); i++) {
		double temp = vertices[i].z;
		vertices[i].z = vertices[i].y;
		vertices[i].y = temp;
	}
}

void Model::reverseZX() {
	for (int i = 0; i < vertices.size(); i++) {
		double temp = vertices[i].x;
		vertices[i].x = vertices[i].z;
		vertices[i].z = temp;
	}
}

void Model::shined(Light& light) {
	Vector3D d = light.getDirection();
	d.x = -d.x;
	d.y = -d.y;
	d.z = -d.z;
	for (int i = 0; i < faces.size(); i++) {
		Vector3D face_normal(0, 0, 0);
		double diff = 0;
		if (normals.size() != 0) {
			int n = 0;
			for (int j = 0; j < 3; j++) {
				face_normal = face_normal + normals[faces[i].normalIndex[j]];
				n += 1;
			}
			face_normal = face_normal / n;
			diff = Vector3D::dot(d, face_normal);
			diff = diff < 0 ? 0 : diff;
		}
		else{
			Vector3D temp0 = vertices[faces[i].vertexIndex[0]] - vertices[faces[i].vertexIndex[1]];
			Vector3D temp1 = vertices[faces[i].vertexIndex[2]] - vertices[faces[i].vertexIndex[1]];
			face_normal = Vector3D::crossProduct(temp0, temp1).normalize();
			diff = Vector3D::dot(d, face_normal);
			diff = diff < 0 ? -diff : diff;
		}
		faces[i].color.green = faces[i].color.green * diff * 0.5 + faces[i].color.green * 0.3;
		faces[i].color.blue = faces[i].color.blue * diff * 0.5 + faces[i].color.blue * 0.3;
		faces[i].color.red = faces[i].color.red * diff * 0.5 + faces[i].color.red * 0.3;
	}
}