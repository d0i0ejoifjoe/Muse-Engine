#pragma once

#include "utils/public/include_glm.h"

namespace muse
{

/**
 *
 *  This class defines a transformation in 3D space
 *  which has translation, rotation, scale
 *  Translation - where to move object from origin of the world.
 *  Rotation - simply the rotation of object.
 *  Scale - scale of the object.
 *
 */
class Transform
{
  public:
    /**
     *
     *  Create a transform.
     *
     */
    Transform();

    /**
     *
     *  Create a transform.
     *
     *  @param translation Translation.
     *  @param rotation Rotation.
     *  @param scale Scale.
     *
     */
    Transform(const glm::vec3 &translation,
              const glm::quat &rotation,
              const glm::vec3 &scale);

    /**
     *
     *  Create a transform.
     *
     *  @param m Matrix.
     *
     */
    explicit Transform(const glm::mat4 &m);

    /**
     *
     *  Interpolate between two transformations.
     *
     *  @param other Other transformation to interpolate with.
     *  @param alpha Amount of interpolation.
     *
     *  @return Result of interpolation.
     *
     */
    Transform interpolate(const Transform &other, float alpha) const;

    /**
     *
     *  Get matrix.
     *
     *  @return Transform matrix.
     *
     */
    glm::mat4 matrix() const;

    /**
     *
     *  Set matrix.
     *
     *  @param m Transform matrix.
     *
     */
    void set_matrix(const glm::mat4 &m);

    /**
     *
     *  Get current translation.
     *
     *  @return Translation.
     *
     */
    glm::vec3 translation() const;

    /**
     *
     *  Get current rotation.
     *
     *  @return Rotation.
     *
     */
    glm::quat rotation() const;

    /**
     *
     *  Get current scale.
     *
     *  @return Scale.
     *
     */
    glm::vec3 scale() const;

    /**
     *
     *  Set scale.
     *
     *  @param scale Scale.
     *
     */
    void set_scale(const glm::vec3 &scale);

    /**
     *
     *  Set rotation.
     *
     *  @param rotation Rotation.
     *
     */
    void set_rotation(const glm::quat &rotation);

    /**
     *
     *  Set translation.
     *
     *  @param translation Translation.
     *
     */
    void set_translation(const glm::vec3 &translation);

  private:
    /** Translation */
    glm::vec3 translation_;

    /** Rotation */
    glm::quat rotation_;

    /** Scale */
    glm::vec3 scale_;
};

}