#include "graphics/public/transform.h"

glm::vec3 lerp(const glm::vec3 &a, const glm::vec3 &b, float alpha)
{
    return a * (1.0f - alpha) + alpha * b;
}

glm::quat slerp(const glm::quat &a, const glm::quat &b, float alpha)
{
    auto dot = glm::dot(a, b);

    float theta = 0.0f;
    float st = 0.0f;
    float sut = 0.0f;
    float sout = 0.0f;
    float coeff1 = 0.0f;
    float coeff2 = 0.0f;

    alpha = alpha / 2.0f;

    theta = std::acos(dot);

    if (theta < 0.0f)
    {
        theta = -theta;
    }

    st = std::sin(theta);
    sut = std::sin(alpha * theta);
    sout = std::sin((1.0f - alpha) * theta);
    coeff1 = sout / st;
    coeff2 = sut / st;

    return glm::normalize(glm::quat{a.w * coeff1 + b.w * coeff2, a.x * coeff1 + b.x * coeff2,
                                    a.y * coeff1 + b.y * coeff2, a.z * coeff1 + b.z * coeff2});
}

namespace muse
{

Transform::Transform(const glm::vec3 &translation, const glm::quat &rotation, const glm::vec3 &scale)
    : translation_(translation)
    , rotation_(rotation)
    , scale_(scale)
{
}

Transform Transform::interpolate(const Transform &other, float alpha) const
{
    return Transform{lerp(translation_, other.translation_, alpha), slerp(rotation_, other.rotation_, alpha),
                     lerp(scale_, other.scale_, alpha)};
}

Transform::Transform(const glm::mat4 &m)
    : Transform()
{
    set_matrix(m);
}

glm::mat4 Transform::matrix() const
{
    return glm::scale(glm::mat4{}, scale_) * glm::translate(glm::mat4{}, translation_) * glm::mat4_cast(rotation_);
}

void Transform::set_matrix(const glm::mat4 &m)
{
    glm::mat4 row_m = glm::transpose(m);

    translation_ = {m[3][0], m[3][1], m[3][2]};

    auto scale_x = glm::length(glm::vec3{m[0][0], m[0][1], m[0][2]});
    auto scale_y = glm::length(glm::vec3{m[1][0], m[1][1], m[1][2]});
    auto scale_z = glm::length(glm::vec3{m[2][0], m[2][1], m[2][2]});

    scale_ = {scale_x, scale_y, scale_z};

    row_m[0][0] /= scale_x;
    row_m[0][1] /= scale_x;
    row_m[0][2] /= scale_x;
    row_m[1][0] /= scale_y;
    row_m[1][1] /= scale_y;
    row_m[1][2] /= scale_y;
    row_m[2][0] /= scale_z;
    row_m[2][1] /= scale_z;
    row_m[2][2] /= scale_z;
    row_m[3][0] = 0.0f;
    row_m[3][1] = 0.0f;
    row_m[3][2] = 0.0f;
    row_m[3][3] = 1.0f;

    glm::quat q{};

    auto trace = m[0][0] + m[1][1] + m[2][2];
    auto root = 0.0f;

    if (trace > 0.0f)
    {
        root = 0.5f / std::sqrt(1.0f + trace);
        q.w = 0.25f / root;
        q.x = (m[2][1] - m[1][2]) * root;
        q.y = (m[0][2] - m[2][0]) * root;
        q.z = (m[1][0] - m[0][1]) * root;
    }
    else if (m[0][0] > m[1][1] && m[0][0] > m[2][2])
    {
        root = 2.0f * std::sqrt(1.0f + m[0][0] - m[1][1] - m[2][2]);
        q.w = (m[2][1] - m[1][2]) / root;
        q.x = 0.25f * root;
        q.y = (m[0][1] + m[1][0]) / root;
        q.z = (m[0][2] + m[2][0]) / root;
    }
    else if (m[1][1] > m[2][2])
    {
        root = 2.0f * std::sqrt(1.0f + m[1][1] - m[0][0] - m[2][2]);
        q.w = (m[0][2] - m[2][0]) / root;
        q.x = (m[0][1] + m[1][0]) / root;
        q.y = 0.25f * root;
        q.z = (m[1][2] + m[2][1]) / root;
    }
    else
    {
        root = 2.0f * std::sqrt(1.0f + m[2][2] - m[0][0] - m[1][1]);
        q.w = (m[1][0] - m[0][1]) / root;
        q.x = (m[0][2] + m[2][0]) / root;
        q.y = (m[1][2] + m[2][1]) / root;
        q.z = 0.25f * root;
    }

    rotation_ = q;
}

glm::vec3 Transform::translation() const
{
    return translation_;
}

glm::quat Transform::rotation() const
{
    return rotation_;
}

glm::vec3 Transform::scale() const
{
    return scale_;
}

void Transform::set_scale(const glm::vec3 &scale)
{
    scale_ = scale;
}

void Transform::set_rotation(const glm::quat &rotation)
{
    rotation_ = rotation;
}

void Transform::set_translation(const glm::vec3 &translation)
{
    translation_ = translation;
}

}