#pragma once

#include "graphics/public/lights/light.h"

namespace muse
{

class SpotLight : public Light
{
  public:
    /**
     *
     *  Create a spot light.
     *
     */
    SpotLight();

    /**
     *
     *  Get type of light.
     *
     *  @return Light type.
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
    glm::mat4 projection() const override;

    /**
     *
     *  Get view matrix.
     *
     *  @return View matrix.
     *
     */
    glm::mat4 view() const override;

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
    void set_direction(const glm::vec3 &direction) override;

    /**
     *
     *  Get position of light.
     *
     *  @return Position vector.
     *
     */
    glm::vec3 position() const override;

    /**
     *
     *  Set position.
     *
     *  @param position New position.
     *
     */
    void set_position(const glm::vec3 &position) override;

    /**
     *
     *  Get attenuation coefficients.
     *
     *  @return Const reference to attenuation coefficients.
     *
     */
    const Attenuation &attenuation() const override;

    /**
     *
     *  Set attenuation.
     *
     *  @param attenuation New attenuation.
     *
     */
    void set_attenuation(const Attenuation &attenuation) override;

  private:
    /** Position. */
    glm::vec3 position_;

    /** Direction. */
    glm::vec3 direction_;

    /** Projection matrix. */
    glm::mat4 projection_;

    /** View matrix. */
    glm::mat4 view_;

    /** Whether light casts shadows. */
    bool casts_shadows_;

    /** Attenuation. */
    Attenuation attenuation_;
};

}