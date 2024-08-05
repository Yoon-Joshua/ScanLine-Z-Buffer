#include<opencv2/opencv.hpp>
#include<iostream>
#include<ctime>
#include"Model.h"
#include"Z_buffer.h"
#include"Light.h"

#define WIDTH 700
#define HEIGHT 700

int main(int argc,char* argv[]) {

	Model model;
	model.load("F:\\model\\miku.obj");

	Light light;
	light.setDirection(Vector3D(-1, -1, -1));
	model.shined(light);

	Z_buffer *z = new Z_buffer(WIDTH, HEIGHT);
	z->setBackground(Vector3D(0, 0, 0));
	cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
	cv::Mat *img;

	int angle = 0;
	int press;
	while (true) {
		clock_t start = clock();
		z->loadMesh(model, angle);
		z->scan();
		img = new cv::Mat(WIDTH, HEIGHT, CV_8UC3, z->getFrame());
		cv::imshow("Display window", *img);
		clock_t end = clock();
		std::cout << "Time: " << end - start << "ms" << std::endl;
		press = cv::waitKey(0);
		if (press == 'A' || press == 'a') {
			angle -= 5;
			angle = angle < 0 ? angle + 360 : angle;
		}
		else if (press == 'D' || press == 'd') {
			angle += 5;
			angle %= 360;
		}
		else break;
		delete img;
	}

	delete z;
	std::cout << "Over" << std::endl;
}