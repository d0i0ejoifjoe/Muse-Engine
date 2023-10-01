#include "graphics/public/lights/direction_light.h"

namespace muse
{
    DirectionalLight::DirectionalLight()
        : direction_(glm::normalize({0.0f, -1.0f, 0.0f}))
        , projection_(glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 0.1, 1000.0f))
        , view_(glm::lookAt(-direction_, {}, {0.0f, 1.0f, 0.0f}))
        , casts_shadows_(true)
    {
    }

    LightType DirectionalLight::type() const
    {
        return LightType::DIRECTIONAL;
    }

    glm::mat4 DirectionalLight::projection() const
    {
        return projection_;
    }

    glm::mat4 DirectionalLight::view() const
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

    void DirectionalLight::set_direction(const glm::vec3& direction)
    {
        direction_ = direction;
        view_ = glm::lookAt(-direction_, {}, {0.0f, 1.0f, 0.0f});
    }

    glm::vec3 DirectionalLight::direction() const
    {
        return direction_;
    }
}