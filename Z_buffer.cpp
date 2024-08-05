#include "Z_buffer.h"
#include <vector>
#include <cmath>

#define PI 3.141592654

Z_buffer::~Z_buffer() {
	delete[] depthBuffer;
	delete[] colorBuffer;
	delete[] frame;
	delete[] edges;
	delete[] polygons;
}

Z_buffer::Z_buffer(int w, int h) :height(h), width(w) {
	depthBuffer = new double[w];
	colorBuffer = new unsigned char[w * 3];
	frame = new unsigned char[w*h * 3];
	edges = new std::vector<int>[h];
	polygons = new std::vector<int>[h];
}

void Z_buffer::setBackground(Vector3D color) {
	background = color;
}

void Z_buffer::loadMesh(Model& m, double time) {
	clear();
	int faceNum = m.getFacesNum();
	int edgeNum = 0;
	int polygonNum = 0;

	time = time * PI / 180;

	for (int i = 0; i < faceNum; i++) {
		Triangle t = m.getTriangle(i);
		for (int j = 0; j < 3; j++) {
			t.vertices[j] = t.vertices[j] * (width / m.span);
			Vector3D temp;
			temp.x = t.vertices[j].x*cos(time) + t.vertices[j].z*sin(time);
			temp.z = t.vertices[j].z*cos(time) - t.vertices[j].x*sin(time);
			temp.y = t.vertices[j].y;
			temp = temp + width / 2;
			t.vertices[j] = temp;
		}
		
		Polygon p;
		double p_ymax = DBL_MIN;
		double p_ymin = DBL_MAX;
		p.id = polygonNum;
		p.plane = calculatePlaneEquation(t.vertices[0], t.vertices[1], t.vertices[2]);
		p.color = t.color;

		for (int j = 0; j < 3; j++) {
			Edge e;
			e.x = t.vertices[j].y < t.vertices[(j + 1) % 3].y ? t.vertices[(j + 1) % 3].x : t.vertices[j].x;
			e.polygonID = polygonNum;
			e.ymax = t.vertices[j].y < t.vertices[(j + 1) % 3].y ? t.vertices[(j + 1) % 3].y : t.vertices[j].y;
			e.ymin = t.vertices[j].y > t.vertices[(j + 1) % 3].y ? t.vertices[(j + 1) % 3].y : t.vertices[j].y;
			e.increment = (t.vertices[j].x - t.vertices[(j + 1) % 3].x) / (t.vertices[(j + 1) % 3].y - t.vertices[j].y);

			// 剔除水平的边
			if (e.increment > 10 || e.increment < -10) {
				continue;
			}

			all_edges.emplace_back(e);
			edges[(int)round(e.ymax)].emplace_back(edgeNum);

			p.edges.emplace_back(edgeNum++);
			if (t.vertices[j].y > p_ymax) p_ymax = t.vertices[j].y;
			if (t.vertices[j].y < p_ymin) p_ymin = t.vertices[j].y;
		}
		p.ymax = p_ymax;
		p.ymin = p_ymin;
		polygons[(int)round(p.ymax)].emplace_back(polygonNum);
		all_polygons.emplace_back(p);

		polygonNum += 1;
	}
}

Plane Z_buffer::calculatePlaneEquation(Vector3D &p, Vector3D &q, Vector3D &r) {
	Plane ret;
	Vector3D pq = q - p;
	Vector3D pr = r - p;
	Vector3D normal = Vector3D::crossProduct(pq, pr);
	ret.a = normal.x;
	ret.b = normal.y;
	ret.c = normal.z;
	ret.d = -p.x*normal.x - p.y*normal.y - p.z*normal.z;
	return ret;
}

void Z_buffer::scan() {
	for (int h = height - 1; h >= 0; h--) {

		// 检查活性表是否要更新
		for (int i = 0; i < active_edges.size(); i += 2) {
			if (all_edges[active_edges[i]].ymin > h&&all_edges[active_edges[i + 1]].ymin > h) {
				active_edges.erase(active_edges.begin() + i, active_edges.begin() + i + 2);
				i -= 2;
			}
			else if (all_edges[active_edges[i]].ymin > h) {
				int pID = all_edges[active_edges[i]].polygonID;

				// 如果该多边形只有2条边，说明它有一条边是水平边
				if (all_polygons[pID].edges.size() == 2) {
					active_edges.erase(active_edges.begin() + i, active_edges.begin() + i + 2);
					i -= 2;
				}
				else {
					for (std::vector<int>::iterator iter = all_polygons[pID].edges.begin(); iter != all_polygons[pID].edges.end(); iter++) {
						if ((int)round(all_edges[*iter].ymax) == h + 1) {
							active_edges[i] = *iter;
							break;
						}
					}
					all_edges[active_edges[i + 1]].x += all_edges[active_edges[i + 1]].increment;
					all_edges[active_edges[i]].x += all_edges[active_edges[i]].increment;
				}
			}
			else if (all_edges[active_edges[i + 1]].ymin > h) {
				int pID = all_edges[active_edges[i + 1]].polygonID;

				// 如果该多边形只有2条边，说明它有一条边是水平边
				if (all_polygons[pID].edges.size() == 2) {
					active_edges.erase(active_edges.begin() + i, active_edges.begin() + i + 2);
					i -= 2;
				}
				else {
					for (std::vector<int>::iterator iter = all_polygons[pID].edges.begin(); iter != all_polygons[pID].edges.end(); iter++) {
						if ((int)round(all_edges[*iter].ymax) == h + 1) {
							active_edges[i + 1] = *iter;
							break;
						}
					}
					all_edges[active_edges[i]].x += all_edges[active_edges[i]].increment;
					all_edges[active_edges[i + 1]].x += all_edges[active_edges[i + 1]].increment;
				}
			}
			else {
				all_edges[active_edges[i]].x += all_edges[active_edges[i]].increment;
				all_edges[active_edges[i + 1]].x += all_edges[active_edges[i + 1]].increment;
			}
		}
		for (int i = 0; i < active_polygons.size();) {
			if (all_polygons[active_polygons[i]].ymin > h) {
				active_polygons.erase(active_polygons.begin() + i);
			}
			else i++;
		}

		// 活性表加入新多边形（同时加入相应的新边）
		for (int i = 0; i < polygons[h].size(); i++) {

			// add new polygons to active polygon table
			active_polygons.emplace_back(polygons[h][i]);

			// add new edge pair to active edge table
			std::vector<int> temp;
			int pID = polygons[h][i];

			if (all_polygons[pID].edges.size() == 3) {
				for (int j = 0; j < 3; j++) {
					int eID = all_polygons[pID].edges[j];
					if ((int)round(all_edges[eID].ymax) == h) {
						temp.emplace_back(eID);
					}
				}
			}
			else if (all_polygons[pID].edges.size() == 2) {
				for (int j = 0; j < 2; j++) {
					int eID = all_polygons[pID].edges[j];
					temp.emplace_back(eID);
				}
			}
			else {
				continue;
			}

			// 在一个边对中，将左边的边置于较小的索引的位置上
			if (all_edges[temp[0]].increment > all_edges[temp[1]].increment) {
				int t = temp[0];
				temp[0] = temp[1];
				temp[1] = t;
			}

			// 在一个边对中，将左边的边置于较小的索引的位置上。（应对某些多边形只有2条边，且呈倒“人”字形的情况）
			if (all_edges[temp[0]].x > all_edges[temp[1]].x) {
				int t = temp[0];
				temp[0] = temp[1];
				temp[1] = t;
			}
			active_edges.emplace_back(temp[0]);
			active_edges.emplace_back(temp[1]);
		}

		// 绘制并测试深度
		for (int i = 0; i < width; i++) {
			depthBuffer[i] = DBL_MIN;
			colorBuffer[i] = 0;
			colorBuffer[i + width] = 0;
			colorBuffer[i + width * 2] = 0;
		}
		for (int i = 0; i < active_edges.size(); i += 2) {
			int start = round(all_edges[active_edges[i]].x);
			int end = round(all_edges[active_edges[i + 1]].x);
			start = start < 0 ? 0 : start;
			start = start >= width ? width - 1 : start;
			end = end < 0 ? 0 : end;
			end = end >= width ? width - 1 : end;

			int pID = all_edges[active_edges[i]].polygonID;
			Plane plane = all_polygons[pID].plane;
			double dz_dx = (-plane.a) / plane.c;

			double depth = (-plane.d - plane.a*all_edges[active_edges[i]].x - plane.b*h) / plane.c;
			for (int j = start; j <= end; j++) {
				if (depth > depthBuffer[j]) {
					colorBuffer[j * 3] = all_polygons[pID].color.red;
					colorBuffer[j * 3 + 1] = all_polygons[pID].color.green;
					colorBuffer[j * 3 + 2] = all_polygons[pID].color.blue;
					depthBuffer[j] = depth;
				}
				depth += dz_dx;
			}
		}
		memcpy(frame + (height - 1 - h)*width * 3, colorBuffer, width * 3 * sizeof(unsigned char));
	}
}

unsigned char* Z_buffer::getFrame() {
	return frame;
}

void Z_buffer::clear() {
	all_edges.clear();
	all_polygons.clear();
	for (int i = 0; i < height; i++) {
		edges[i].clear();
		polygons[i].clear();
	}
	active_edges.clear();
	active_polygons.clear();
}
