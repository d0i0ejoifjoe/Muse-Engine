#pragma once

#include "utils/public/matrix4.h"
#include "utils/public/vector3.h"

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
    Transform(const Vector3 &translation,
              const Quaternion &rotation,
              const Vector3 &scale);

    /**
     *
     *  Create a transform.
     *
     *  @param m Matrix.
     *
     */
    explicit Transform(const Matrix4 &m);

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
    Matrix4 matrix() const;

    /**
     *
     *  Set matrix.
     *
     *  @param m Transform matrix.
     *
     */
    void set_matrix(const Matrix4 &m);

    /**
     *
     *  Get current translation.
     *
     *  @return Translation.
     *
     */
    Vector3 translation() const;

    /**
     *
     *  Get current rotation.
     *
     *  @return Rotation.
     *
     */
    Quaternion rotation() const;

    /**
     *
     *  Get current scale.
     *
     *  @return Scale.
     *
     */
    Vector3 scale() const;

    /**
     *
     *  Set scale.
     *
     *  @param scale Scale.
     *
     */
    void set_scale(const Vector3 &scale);

    /**
     *
     *  Set rotation.
     *
     *  @param rotation Rotation.
     *
     */
    void set_rotation(const Quaternion &rotation);

    /**
     *
     *  Set translation.
     *
     *  @param translation Translation.
     *
     */
    void set_translation(const Vector3 &translation);

  private:
    /** Translation */
    Vector3 translation_;

    /** Rotation */
    Quaternion rotation_;

    /** Scale */
    Vector3 scale_;
};

}