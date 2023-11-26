#pragma once

#include "graphics/public/lights/light.h"
#include "utils/public/matrix4.h"

namespace muse
{

/**
 *
 *  Directional light implementation.
 *
 */
class DirectionalLight : public Light
{
  public:
    /**
     *
     *  Create a directional light.
     *
     */
    DirectionalLight();

    /**
     *
     *  Get light's type.
     *
     *  @return Light's type.
     *
     */
    LightType type() const override;

    /**
     *
     *  Get projection matrix.
     *
     *  @return Projection matrix.
     *
     */
    Matrix4 projection() const override;

    /**
     *
     *  Get view matrix.
     *
     *  @return View matrix.
     *
     */
    Matrix4 view() const override;

    /**
     *
     *  Get whether light casts shadows.
     *
     *  @return True if it is, false otherwise.
     *
     */
    bool casts_shadows() const override;

    /**
     *
     *  Set whether light will cast shadows.
     *
     *  @param casts_shadows Whether light will cast shadows.
     *
     */
    void set_casts_shadows(bool casts_shadows) override;

    /**
     *
     *  Set direction.
     *
     *  @param direction New direction.
     *
     */
    void set_direction(const Vector3 &direction) override;

    /**
     *
     *  Get light's direction.
     *
     *  @return Direction vector.
     *
     */
    Vector3 direction() const override;

  private:
    /** Light's direction. */
    Vector3 direction_;

    /** Projection matrix. */
    Matrix4 projection_;

    /** View matrix. */
    Matrix4 view_;

    /** Whether light casts shadows. */
    bool casts_shadows_;
};

}