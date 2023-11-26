#pragma once

#include "utils/public/matrix4.h"

#include <cstdint>

namespace muse
{

/** Enum for all camera types */
enum class CameraType : std::uint8_t
{
    ORTHOGRAPHIC,
    PERSPECTIVE,
    INFINITE_PERSPECTIVE
};

class Window;

/**
 *
 *  Class that represents a camera.
 *  Camera is just a set of matrices that helps us transform vertices in vertex shader.
 *  First we used view matrix to transform ourselves into view space and then we use
 *  projection matrix to transform us into clip space then perspective division, clipping and so on.
 *  So in layman's terms this class helps us to make things look more natural on screen.
 *
 */
class Camera
{
  public:
    /**
     *
     *  Create a camera.
     *
     *  @param type Type of camera.
     *  @param width Width of projection.
     *  @param height Height of projection.
     *  @param depth Depth of projection.
     *
     */
    Camera(CameraType type, std::uint32_t width, std::uint32_t height, float depth);

    /**
     *
     *  Get yaw of camera.
     *
     *  @return Yaw angle.
     *
     */
    float yaw() const;

    /**
     *
     *  Set yaw of camera.
     *
     *  @param yaw New yaw.
     *
     */
    void set_yaw(float yaw);

    /**
     *
     *  Adjust yaw of camera.
     *
     *  @param yaw Yaw to adjust.
     *
     */
    void adjust_yaw(float yaw);

    /**
     *
     *  Get pitch of camera.
     *
     *  @return Pitch angle.
     *
     */
    float pitch() const;

    /**
     *
     *  Set pitch of camera.
     *
     *  @param pitch New pitch.
     *
     */
    void set_pitch(float pitch);

    /**
     *
     *  Adjust pitch of camera.
     *
     *  @param pitch Pitch to adjust.
     *
     */
    void adjust_pitch(float pitch);

    /**
     *
     *  Get projection matrix.
     *
     *  @return Projection matrix.
     *
     */
    Matrix4 projection() const;

    /**
     *
     *  Get view matrix.
     *
     *  @return View matrix.
     *
     */
    Matrix4 view() const;

    /**
     *
     *  Get position of camera in world space.
     *
     *  @return Position.
     *
     */
    Vector3 position() const;

    /**
     *
     *  Set position of camera.
     *
     *  @param position New position.
     *
     */
    void set_position(const Vector3 &position);

    /**
     *
     *  Translate camera of it's current position.
     *
     *  @param translation Amount of translation.
     *
     */
    void translate(const Vector3 &translation);

    /**
     *
     *  Get up vector.
     *
     *  @return Up vector.
     *
     */
    Vector3 up() const;

    /**
     *
     *  Get right vector.
     *
     *  @return Right vector.
     *
     */
    Vector3 right() const;

    /**
     *
     *  Get camera's field of view.
     *
     *  @return FOV.
     *
     */
    float fov() const;

    /**
     *
     *  Set field of view.
     *
     *  @param fov New FOV.
     *
     */
    void set_fov(float fov);

    /**
     *
     *  Adjust field of view.
     *
     *  @param fov FOV to adjust with.
     *
     */
    void adjust_fov(float fov);

    /**
     *
     *  Get camera's projection width.
     *
     *  @return Width.
     *
     */
    float width() const;

    /**
     *
     *  Get camera's projection height.
     *
     *  @return Height.
     *
     */
    float height() const;

    /**
     *
     *  Set camera's projection width.
     *
     *  @param width New width.
     *
     */
    void set_width(float width);

    /**
     *
     *  Set camera's projection height.
     *
     *  @param height New height.
     *
     */
    void set_height(float height);

    /**
     *
     *  Get camera's direction.
     *
     *  @return Direction vector.
     *
     */
    Vector3 direction() const;

    /**
     *
     *  Set camera's projection type.
     *
     *  @param type Camera type.
     *
     */
    void set_type(CameraType type);

    /**
     *
     *  Get camera's projection type.
     *
     *  @return Camera's type.
     *
     */
    CameraType type() const;

  private:
    /**
     *
     *  Helper for direction recreation.
     *
     */
    void recreate_direction();

    /**
     *
     *  Helper for projection matrix recreation.
     *
     */
    void recreate_proj();

    /**
     *
     *  Helper for view matrix recreation.
     *
     */
    void recreate_view();

    /** Yaw of orientation */
    float yaw_;

    /** Pitch of orientation */
    float pitch_;

    /** Field of view */
    float fov_;

    /** Width of projection matrix */
    float width_;

    /** Height of projection matrix */
    float height_;

    /** Depth of projection matrix */
    float depth_;

    /** Projection matrix */
    Matrix4 projection_;

    /** View matrix */
    Matrix4 view_;

    /** Positon of camera in world space */
    Vector3 position_;

    /** Direction of camera */
    Vector3 direction_;

    /** Up vector */
    Vector3 up_;

    /** Type of projection matrix */
    CameraType type_;
};

}