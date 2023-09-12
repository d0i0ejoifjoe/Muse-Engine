#pragma once

#include "utils/public/include_glm.h"

#include <cstdint>

namespace muse
{
    /** Enum for all camera types */
    enum class CameraType : std::uint8_t
    {   
        ORTHOGRAPHIC,
        PERSPECTIVE
    };

    class Window;

    /**
     * 
     *  Class that represents a camera.
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
         *  @param window Reference to window.
         *  @param depth Depth of projection.
         * 
        */
        Camera(CameraType type, Window& window, float depth);
    
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
        glm::mat4 projection() const;

        /**
         * 
         *  Get view matrix.
         * 
         *  @return View matrix.
         * 
        */
        glm::mat4 view() const;

        /**
         * 
         *  Get position of camera in world space.
         * 
         *  @return Position.
         * 
        */
        glm::vec3 position() const;

        /**
         * 
         *  Set position of camera.
         * 
         *  @param position New position.
         * 
        */
        void set_position(const glm::vec3& position);

        /**
         * 
         *  Translate camera of it's current position.
         * 
         *  @param translation Amount of translation.
         * 
        */
        void translate(const glm::vec3& translation);

        /**
         * 
         *  Get up vector.
         * 
         *  @return Up vector.
         * 
        */
        glm::vec3 up() const;

        /**
         * 
         *  Get right vector.
         * 
         *  @return Right vector.
         * 
        */
        glm::vec3 right() const;

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
        glm::mat4 projection_;

        /** View matrix */
        glm::mat4 view_;

        /** Positon of camera in world space */
        glm::vec3 position_;

        /** Direction of camera */
        glm::vec3 direction_;

        /** Up vector */
        glm::vec3 up_;

        /** Type of projection matrix */
        CameraType type_;
    };
}