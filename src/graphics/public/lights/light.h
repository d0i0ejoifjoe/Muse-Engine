#pragma once

#include "utils/public/matrix4.h"

namespace muse
{

/**
 *
 *  Structure that encapsulates all of attenuation coefficients.
 *
 */
struct Attenuation
{
    /** Constant coefficient */
    float constant;

    /** Quadratic coefficient */
    float quadratic;

    /** Linear coefficient */
    float linear;
};

/** Enum for all different types of light. */
enum class LightType : std::uint8_t
{
    AMBIENT,
    DIRECTIONAL,
    POINT,
    SPOT
};

/**
 *
 *  Base class for all types of light.
 *
 */
class Light
{
  public:
    /**
     *
     *  Create a light.
     *
     */
    Light();

    /** Default destructor */
    virtual ~Light() = default;

    /**
     *
     *  Get type of light.
     *
     *  @return Light type.
     *
     */
    virtual LightType type() const = 0;

    /**
     *
     *  Get light's intensity.
     *
     *  @return Intensity value.
     *
     */
    float intensity() const;

    /**
     *
     *  Set light's intensity.
     *
     *  @param intensity New intensity.
     *
     */
    void set_intensity(float intensity);

    /**
     *
     *  Get light's color.
     *
     *  @return Color vector.
     *
     */
    Vector4 color() const;

    /**
     *
     *  Set light's color.
     *
     *  @param color New color.
     *
     */
    void set_color(const Vector4 &color);

    /**
     *
     *  Get projection matrix.
     *
     *  @return Projection matrix.
     *
     */
    virtual Matrix4 projection() const = 0;

    /**
     *
     *  Get view matrix.
     *
     *  @return View matrix.
     *
     */
    virtual Matrix4 view() const = 0;

    /**
     *
     *  Get whether light casts shadows.
     *
     *  @return True if it is, false otherwise.
     *
     */
    virtual bool casts_shadows() const = 0;

    /**
     *
     *  Set whether light will cast shadows.
     *
     *  @param casts_shadows Whether light will cast shadows.
     *
     */
    virtual void set_casts_shadows(bool casts_shadows) = 0;

    /**
     *
     *  Set direction.
     *
     *  @param direction New direction.
     *
     */
    virtual void set_direction(const Vector3 &direction) = 0;

    /**
     *
     *  Get direction of light.
     *
     *  @return Direction vector.
     *
     */
    virtual Vector3 direction() const = 0;

    /**
     *
     *  Get position of light.
     *
     *  @return Position vector.
     *
     */
    virtual Vector3 position() const = 0;

    /**
     *
     *  Set position.
     *
     *  @param position New position.
     *
     */
    virtual void set_position(const Vector3 &position) = 0;

    /**
     *
     *  Get attenuation coefficients.
     *
     *  @return Const reference to attenuation coefficients.
     *
     */
    virtual const Attenuation &attenuation() const = 0;

    /**
     *
     *  Set attenuation.
     *
     *  @param attenuation New attenuation.
     *
     */
    virtual void set_attenuation(const Attenuation &attenuation) = 0;

  protected:
    /** Light intensity. */
    float intensity_;

    /** Light color. */
    Vector4 color_;
};

}