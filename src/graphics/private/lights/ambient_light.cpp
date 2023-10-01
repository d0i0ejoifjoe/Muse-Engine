#include "graphics/public/lights/ambient_light.h"

namespace muse
{
    LightType AmbientLight::type() const
    {
        return LightType::AMBIENT;
    }
}