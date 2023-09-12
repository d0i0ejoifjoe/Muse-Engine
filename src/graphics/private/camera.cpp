#include "graphics/public/camera.h"
#include "graphics/public/window.h"
#include "utils/public/utils.h"

namespace muse
{
    Camera::Camera(CameraType type, Window& window, float depth)
        : yaw_(-pi / 2.0f)
        , pitch_(0.0f)
        , fov(45.0f)
        , width_(0.0f)
        , height_(0.0f)
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
    }

    void Camera::recreate_direction()
    {
        direction_.x = std::cos(yaw_) * std::cos(pitch_);
        direction_.y = std::sin(pitch_);
        direction_.z = std::sin(yaw_) * std::cos(pitch_);

        direction_ = glm::normalize(direction_);
    }

    void Camera::recreate_proj()
    {
        if(type_ == CameraType::ORTHOGRAPHIC)
        {
            projection_ = glm::ortho(-width_, width_, -height_, height_, -depth, depth);
        }
        else if(type_ == CameraType::PERSPECTIVE)
        {
            projection_ = glm::perspective(glm::radians(fov_), width_ / height_, 0.1f, depth);
        }
    }

    void Camera::recreate_view()
    {
        recreate_direction();
        view_ = glm::lookAt(position_, position_ + direction_, up_);
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

    glm::mat4 Camera::projection() const
    {
        return projection_;
    }

    glm::mat4 Camera::view() const
    {
        return view_;
    }

    glm::vec3 Camera::position() const
    {
        return position_;
    }

    void Camera::set_position(const glm::vec3& position)
    {
        position_ = position;
        recreate_view();
    }

    void Camera::translate(const glm::vec3& translation)
    {
        position_ += translation;
        recreate_view();
    }

    glm::vec3 Camera::up() const
    {
        return up_;
    }

    glm::vec3 Camera::right() const
    {
        return glm::normalize(glm::cross(up_, direction_));
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
}