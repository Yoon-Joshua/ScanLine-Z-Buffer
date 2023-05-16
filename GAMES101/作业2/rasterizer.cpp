// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}


static bool insideTriangle(int x, int y, const Vector3f* _v)
{   
    // TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]

    float alpha = (-(x - _v[1].x()) * (_v[2].y() - _v[1].y()) + (y - _v[1].y()) * (_v[2].x() - _v[1].x())) / (-(_v[0].x() - _v[1].x()) * (_v[2].y() - _v[1].y()) + (_v[0].y() - _v[1].y()) * (_v[2].x() - _v[1].x()));
    float beta = (-(x - _v[2].x()) * (_v[0].y() - _v[2].y()) + (y - _v[2].y()) * (_v[0].x() - _v[2].x())) / (-(_v[1].x() - _v[2].x()) * (_v[0].y() - _v[2].y()) + (_v[1].y() - _v[2].y()) * (_v[0].x() - _v[2].x()));
    float gamma = 1 - alpha - beta;
    if (alpha > 0 && beta > 0 && gamma > 0) return true;
    else return false;
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
{
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;
        Eigen::Vector4f v[] = {
                mvp * to_vec4(buf[i[0]], 1.0f),
                mvp * to_vec4(buf[i[1]], 1.0f),
                mvp * to_vec4(buf[i[2]], 1.0f)
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec /= vec.w();
        }
        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        rasterize_triangle(t);
    }
}

void rst::rasterizer::ssaa_draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;
        Eigen::Vector4f v[] = {
                mvp * to_vec4(buf[i[0]], 1.0f),
                mvp * to_vec4(buf[i[1]], 1.0f),
                mvp * to_vec4(buf[i[2]], 1.0f)
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec /= vec.w();
        }
        //Viewport transformation
        for (auto& vert : v)
        {
            vert.x() = 0.5 * width * 2 * (vert.x() + 1.0);
            vert.y() = 0.5 * height * 2 * (vert.y() + 1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        ssaa_rasterize_triangle(t);
    }
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            frame_buf[j * width + i] = ssaa_frame_buf[2 * j * 2 * width + 2 * i]
                + ssaa_frame_buf[2 * j * 2 * width + 2 * i + 1]
                + ssaa_frame_buf[(2 * j + 1) * 2 * width + 2 * i]
                + ssaa_frame_buf[(2 * j + 1) * 2 * width + 2 * i + 1];
            frame_buf[j * width + i] /= 4;
        }
    }
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t) {
    auto v = t.toVector4();
    
    // Find out the bounding box of current triangle.
    float xmin = FLT_MAX;
    float ymin = FLT_MAX;
    float xmax = FLT_MIN;
    float ymax = FLT_MIN;
    for (auto e : v) {
        xmin = std::min(xmin, e(0, 0));
        ymin = std::min(ymin, e(1, 0));
        xmax = std::max(xmax, e(0, 0));
        ymax = std::max(ymax, e(1, 0));
    }
    xmin = xmin < width ? xmin : width - 0.01;
    xmax = xmax < width ? xmax : width - 0.01;
    xmin = xmin >= 0 ? xmin : 0;
    xmax = xmax >= 0 ? xmax : 0;
    ymin = ymin < height ? ymin : height - 0.01;
    ymax = ymax < height ? ymax : height - 0.01;
    ymin = ymin >= 0 ? ymin : 0;
    ymax = ymax >= 0 ? ymax : 0;

    // iterate through the pixel and find if the current pixel is inside the triangle
    // If so, use the following code to get the interpolated z value.
    //auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
    //float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
    //float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
    //z_interpolated *= w_reciprocal;

    // TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.
    int xmin_pix = floorf(xmin);
    int ymin_pix = floorf(ymin);
    int xmax_pix = floorf(xmax);
    int ymax_pix = floorf(ymax);
    for (int i = xmin_pix; i <= xmax_pix; ++i) {
        for (int j = ymin_pix; j <= ymax_pix; ++j) {
            if (!insideTriangle(i + 0.5, j + 0.5, t.v)) continue;
            auto [alpha, beta, gamma] = computeBarycentric2D(i + 0.5, j + 0.5, t.v);
            float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
            float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
            z_interpolated *= w_reciprocal;
            if (z_interpolated > depth_buf[i * width + j]) {
                depth_buf[i * width + j] = z_interpolated;
                set_pixel(Eigen::Vector3f(i, j, 1), t.getColor());
            }
        }
    }
}

void rst::rasterizer::ssaa_rasterize_triangle(const Triangle& t) {
    auto v = t.toVector4();
    float h = 2 * height;
    float w = 2 * width;

    // Find out the bounding box of current triangle.
    float xmin = FLT_MAX;
    float ymin = FLT_MAX;
    float xmax = FLT_MIN;
    float ymax = FLT_MIN;
    for (auto e : v) {
        xmin = std::min(xmin, e(0, 0));
        ymin = std::min(ymin, e(1, 0));
        xmax = std::max(xmax, e(0, 0));
        ymax = std::max(ymax, e(1, 0));
    }
    xmin = xmin < w ? xmin : w - 0.01;
    xmax = xmax < w ? xmax : w - 0.01;
    xmin = xmin >= 0 ? xmin : 0;
    xmax = xmax >= 0 ? xmax : 0;
    ymin = ymin < h ? ymin : h - 0.01;
    ymax = ymax < h ? ymax : h - 0.01;
    ymin = ymin >= 0 ? ymin : 0;
    ymax = ymax >= 0 ? ymax : 0;

    int xmin_pix = floorf(xmin);
    int ymin_pix = floorf(ymin);
    int xmax_pix = floorf(xmax);
    int ymax_pix = floorf(ymax);
    for (int i = xmin_pix; i <= xmax_pix; ++i) {
        for (int j = ymin_pix; j <= ymax_pix; ++j) {
            if (!insideTriangle(i + 0.5, j + 0.5, t.v)) continue;
            auto [alpha, beta, gamma] = computeBarycentric2D(i + 0.5, j + 0.5, t.v);
            float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
            float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
            z_interpolated *= w_reciprocal;
            if (z_interpolated > ssaa_depth_buf[i * w + j]) {
                ssaa_depth_buf[i * w + j] = z_interpolated;
                ssaa_set_pixel(Eigen::Vector3f(i, j, 1), t.getColor());
            }
        }
    }
}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
        std::fill(ssaa_frame_buf.begin(), ssaa_frame_buf.end(), Eigen::Vector3f{ 0, 0, 0 });
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
        std::fill(depth_buf.begin(), depth_buf.end(), FLT_MIN);
        std::fill(ssaa_depth_buf.begin(), ssaa_depth_buf.end(), FLT_MIN);
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);
    ssaa_frame_buf.resize(w * h * 4);
    ssaa_depth_buf.resize(w * h * 4);
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-1-y)*width + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height-1-point.y())*width + point.x();
    frame_buf[ind] = color;

}

void rst::rasterizer::ssaa_set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (2 * height - 1 - point.y()) * 2 * width + point.x();
    ssaa_frame_buf[ind] = color;
}

// clang-format on