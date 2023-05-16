#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_rotation(Vector3f axis, float angle) {
    Eigen::Matrix4f rotation = Eigen::Matrix4f::Identity();
    angle = angle * MY_PI / 180;
    Eigen::Vector3f w = axis;
    w.normalize();
    Eigen::Vector3f u;
    Eigen::Vector3f v;
    Eigen::Vector3f t;
    while (true) {
        t = Eigen::Vector3f::Random();
        u = t.cross(w);
        if (u.norm() > 0.01) break;
    }
    u.normalize();
    v = w.cross(u);
    v.normalize();

    Eigen::Matrix4f R;
    R << u.x(), u.y(), u.z(), 0,
        v.x(), v.y(), v.z(), 0,
        w.x(), w.y(), w.z(), 0,
        0, 0, 0, 1;
    Eigen::Matrix4f rotate_z;
    rotate_z << cosf(angle), -sinf(angle), 0, 0,
        sinf(angle), cosf(angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1;
    rotation = R.transpose() * rotate_z * R;
    return rotation;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    float angle = rotation_angle * MY_PI / 180;
    
    model <<
        cosf(angle), -sinf(angle), 0, 0,
        sinf(angle), cosf(angle), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1;

    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    float h = 2 * (zNear)*tanf(eye_fov * MY_PI / 360);
    float w = aspect_ratio * h;
    float l = -w / 2;
    float r = w / 2;
    float t = h / 2;
    float b = -h / 2;
    float n = -zNear;
    float f = -zFar;

    // 透视变换，把视锥体变换成轴对齐长方体
    Eigen::Matrix4f perspective = Eigen::Matrix4f::Identity();
    // 变换到裁剪空间，将轴对齐长方体变换轴对齐为正方体，各维度坐标范围均为[-1, 1]。
    Eigen::Matrix4f orthographic = Eigen::Matrix4f::Identity();

    perspective <<
        n, 0, 0, 0,
        0, n, 0, 0,
        0, 0, n + f, -f * n,
        0, 0, 1, 0;
    orthographic <<
        2 / (r - l), 0, 0, -(r + l) / (r - l),
        0, 2 / (t - b), 0, -(t + b) / (t - b),
        0, 0, 2 / (n - f), -(n + f) / (n - f),
        0, 0, 0, 1;
    projection = orthographic * perspective;

    return projection;
}

int main(int argc, const char** argv)
{
    float angle = 45;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        //r.set_model(get_model_matrix(angle));
        r.set_model(get_rotation(Eigen::Vector3f(1, 1, 1), angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(27);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
