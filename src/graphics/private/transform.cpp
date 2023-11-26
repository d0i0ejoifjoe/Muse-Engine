#include "graphics/public/transform.h"

#include "log/public/logger.h"

namespace muse
{

Transform::Transform()
    : translation_({0.0f, 0.0f, 0.0f})
    , rotation_({1.0f, 0.0f, 0.0f, 0.0f})
    , scale_({1.0f, 1.0f, 1.0f})
{
}

Transform::Transform(const Vector3 &translation, const Quaternion &rotation, const Vector3 &scale)
    : translation_(translation)
    , rotation_(rotation)
    , scale_(scale)
{
}

Transform Transform::interpolate(const Transform &other, float alpha) const
{
    return Transform{Vector3::lerp(translation_, other.translation_, alpha), Quaternion::slerp(rotation_, other.rotation_, alpha),
                     Vector3::lerp(scale_, other.scale_, alpha)};
}

Transform::Transform(const Matrix4 &m)
    : Transform()
{
    set_matrix(m);
}

Matrix4 Transform::matrix() const
{
    return Matrix4::translate(translation_) * Matrix4::rotate(rotation_) * Matrix4::scale(scale_);
}

void Transform::set_matrix(const Matrix4 &m)
{
    auto [translation, rotation, scale] = Matrix4::decompose(m);

    translation_ = translation;
    rotation_ = rotation;
    scale_ = scale;
}

Vector3 Transform::translation() const
{
    return translation_;
}

Quaternion Transform::rotation() const
{
    return rotation_;
}

Vector3 Transform::scale() const
{
    return scale_;
}

void Transform::set_scale(const Vector3 &scale)
{
    scale_ = scale;
}

void Transform::set_rotation(const Quaternion &rotation)
{
    rotation_ = rotation;
}

void Transform::set_translation(const Vector3 &translation)
{
    translation_ = translation;
}

}