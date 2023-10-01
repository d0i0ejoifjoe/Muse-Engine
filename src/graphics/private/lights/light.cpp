#include "graphics/public/lights/light.h"

namespace muse
{
    Light::Light()
        : projection_({1.0f})
        , view_({1.0f})
        , intensity_(0.5f)
        , color_({1.0f, 1.0f, 1.0f, 1.0f})
    {
    }

    float Light::intensity() const
    {
        return intensity_;
    }

    void Light::set_intensity(float intensity)
    {
        intensity_ = intensity;
    }
    
    glm::mat4 Light::color() const
    {
        return color_;
    }

    void Light::set_color(const glm::vec4& color)
    {
        color_ = color;
    }
}