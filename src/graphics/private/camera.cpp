#include "graphics/public/camera.h"

#include "graphics/public/window.h"
#include "log/public/logger.h"
#include "utils/public/utils.h"

#include <SDL.h>

namespace muse
{

Camera::Camera(CameraType type, std::uint32_t width, std::uint32_t height, float depth)
    : yaw_(-pi / 2.0f)
    , pitch_(0.0f)
    , fov_(45.0f)
    , width_(static_cast<float>(width))
    , height_(static_cast<float>(height))
    , depth_(depth)
    , projection_({1.0f})
    , view_({1.0f})
    , position_({0.0f, 0.0f, 0.0f})
    , direction_({0.0f, 0.0f, -1.0f})
    , up_({0.0f, 1.0f, 0.0f})
    , type_(type)
{
    recreate_proj();
    recreate_view();

    LOG_INFO(Camera, "Camera created!");
}

void Camera::recreate_direction()
{
    const auto yaw_rads = to_radians(yaw_);
    const auto pitch_rads = to_radians(pitch_);

    direction_.x = std::cos(yaw_rads) * std::cos(pitch_rads);
    direction_.y = std::sin(pitch_rads);
    direction_.z = std::sin(yaw_rads) * std::cos(pitch_rads);

    direction_ = Vector3::normalize(direction_);
}

void Camera::recreate_proj()
{
    if (type_ == CameraType::ORTHOGRAPHIC)
    {
        projection_ = Matrix4::orthographic_projection(width_, height_, depth_);
    }
    else if (type_ == CameraType::PERSPECTIVE)
    {
        projection_ = Matrix4::perspective_projection(fov_, width_, height_, 0.1f, depth_);
    }
    else if (type_ == CameraType::INFINITE_PERSPECTIVE)
    {
        // Depth is ignored
        projection_ = Matrix4::infinite_perspective_projection(fov_, width_, height_, 0.1f);
    }
}

void Camera::recreate_view()
{
    recreate_direction();
    view_ = Matrix4::look_at(position_, position_ + direction_, up_);
}

float Camera::yaw() const
{
    return yaw_;
}

void Camera::set_yaw(float yaw)
{
    yaw_ = yaw;
    recreate_view();
}

void Camera::adjust_yaw(float yaw)
{
    yaw_ += yaw;
    recreate_view();
}

float Camera::pitch() const
{
    return pitch_;
}

void Camera::set_pitch(float pitch)
{
    pitch_ = pitch;
    recreate_view();
}

void Camera::adjust_pitch(float pitch)
{
    pitch_ += pitch;
    recreate_view();
}

Matrix4 Camera::projection() const
{
    return projection_;
}

Matrix4 Camera::view() const
{
    return view_;
}

Vector3 Camera::position() const
{
    return position_;
}

void Camera::set_position(const Vector3 &position)
{
    position_ = position;
    recreate_view();
}

void Camera::translate(const Vector3 &translation)
{
    position_ += translation;
    recreate_view();
}

Vector3 Camera::up() const
{
    return up_;
}

Vector3 Camera::right() const
{
    return Vector3::normalize(Vector3::cross(up_, direction_));
}

float Camera::fov() const
{
    return fov_;
}

void Camera::set_fov(float fov)
{
    fov_ = fov;
    recreate_proj();
}

void Camera::adjust_fov(float fov)
{
    fov_ += fov;
    recreate_proj();
}

Vector3 Camera::direction() const
{
    return direction_;
}

float Camera::width() const
{
    return width_;
}

float Camera::height() const
{
    return height_;
}

void Camera::set_width(float width)
{
    width_ = width;
    recreate_proj();
}

void Camera::set_height(float height)
{
    height_ = height;
    recreate_proj();
}

CameraType Camera::type() const
{
    return type_;
}

void Camera::set_type(CameraType type)
{
    type_ = type;
    recreate_proj();
}

}