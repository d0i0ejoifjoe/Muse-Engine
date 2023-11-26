#include "graphics/public/lights/spot_light.h"

namespace muse
{

SpotLight::SpotLight()
    : position_({0.0f, 0.0f, 0.0f})
    , direction_(Vector3::normalize({0.0f, -1.0f, 0.0f}))
    , projection_(Matrix4::perspective_projection(45.0f, 100.0f, 100.0f, 0.1f, 1000.0f))
    , view_(Matrix4::look_at(position_, position_ + direction_, {0.0f, 1.0f, 0.0f}))
    , casts_shadows_(true)
    , attenuation_({1.0f, 0.0014f, 0.000007f})
{
}

LightType SpotLight::type() const
{
    return LightType::SPOT;
}

Matrix4 SpotLight::projection() const
{
    return projection_;
}

Matrix4 SpotLight::view() const
{
    return view_;
}

bool SpotLight::casts_shadows() const
{
    return casts_shadows_;
}

void SpotLight::set_casts_shadows(bool casts_shadows)
{
    casts_shadows_ = casts_shadows;
}

void SpotLight::set_direction(const Vector3 &direction)
{
    direction_ = direction;
    view_ = Matrix4::look_at(position_, position_ + direction_, {0.0f, 1.0f, 0.0f});
}

Vector3 SpotLight::position() const
{
    return position_;
}

void SpotLight::set_position(const Vector3 &position)
{
    position_ = position;
    view_ = Matrix4::look_at(position_, position_ + direction_, {0.0f, 1.0f, 0.0f});
}

const Attenuation &SpotLight::attenuation() const
{
    return attenuation_;
}

void SpotLight::set_attenuation(const Attenuation &attenuation)
{
    attenuation_ = attenuation;
}

}