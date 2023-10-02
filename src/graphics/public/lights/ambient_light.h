#pragma once

#include "graphics/public/lights/light.h"

namespace muse
{

/**
 *
 *  Ambient light implementation.
 *
 */
class AmbientLight : public Light
{
  public:
    /**
     *
     *  Create ambient light.
     *
     */
    AmbientLight() = default;

    /**
     *
     *  Get light's type.
     *
     *  @return Light's type.
     *
     */
    LightType type() const override;
};

}