#include "graphics/public/lights/directional_light.h"

namespace muse
{

DirectionalLight::DirectionalLight()
    : direction_(Vector3::normalize({0.0f, -1.0f, 0.0f}))
    , projection_(Matrix4::orthographic_projection(100.0f, 100.0f, 1000.0f))
    , view_(Matrix4::look_at(-direction_, {}, {0.0f, 1.0f, 0.0f}))
    , casts_shadows_(true)
{
}

LightType DirectionalLight::type() const
{
    return LightType::DIRECTIONAL;
}

Matrix4 DirectionalLight::projection() const
{
    return projection_;
}

Matrix4 DirectionalLight::view() const
{
    return view_;
}

bool DirectionalLight::casts_shadows() const
{
    return casts_shadows_;
}

void DirectionalLight::set_casts_shadows(bool casts_shadows)
{
    casts_shadows_ = casts_shadows;
}

void DirectionalLight::set_direction(const Vector3 &direction)
{
    direction_ = direction;
    view_ = Matrix4::look_at(-direction_, {}, {0.0f, 1.0f, 0.0f});
}

Vector3 DirectionalLight::direction() const
{
    return direction_;
}

}