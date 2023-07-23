//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>
class Texture{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        // 弃用原来的，用双线性插值
        return getBilinear(u, v);
        u = std::clamp<float>(u, 0, 1);
        v = std::clamp<float>(v, 0, 1);

        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

    Eigen::Vector3f getBilinear(float u, float v) {
        u = std::clamp<float>(u, 0, 1);
        v = std::clamp<float>(v, 0, 1);
        float u_img = u * width;
        float v_img = (1 - v) * height;

        int a = std::floor(u_img);
        int b = std::floor(v_img);
        int c = a + 1 < width ? a + 1 : width;
        int d = b + 1 < height ? b + 1 : height;

        auto c0 = image_data.at<cv::Vec3b>(v_img, u_img);
        auto c1 = image_data.at<cv::Vec3b>(b, c);
        auto c2 = image_data.at<cv::Vec3b>(d, a);
        auto c3 = image_data.at<cv::Vec3b>(d, c);

        float p1 = v_img - (float)b;
        float p2 = u_img - (float)a;
        auto temp1 = p1 * c0 + (1 - p1) * c2;
        auto temp2 = p1 * c1 + (1 - p1) * c3;
        auto color = p2 * temp1 + (1 - p2) * temp2;
        
        //return Eigen::Vector3f(c0[0], c0[1], c0[2]);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

};
#endif //RASTERIZER_TEXTURE_H
