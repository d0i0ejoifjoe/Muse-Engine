#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace muse
{
    class Transform
    {
    public:
        /**
         * 
         *  Create a transform.
         * 
        */
        constexpr Transform() = default;

        /**
         * 
         *  Create a transform.
         * 
         *  @param translation Translation.
         *  @param rotation Rotation.
         *  @param scale Scale.
         * 
        */
        constexpr Transform(const glm::vec3& translation,
                            const glm::quat& rotation,
                            const glm::vec3& scale)
            : translation_(translation)
            , rotation_(rotation)
            , scale_(scale)
        {
        }

        /**
         * 
         *  Create a transform.
         * 
         *  @param m Matrix.
         * 
        */
        constexpr Transform(const glm::mat4& m)
            : Transform()
        {
            set_matrix(m);
        }

        /**
         * 
         *  Get matrix.
         * 
         *  @return Transform matrix.  
         * 
        */
        constexpr glm::mat4 matrix() const
        {
            return glm::scale(glm::mat4{}, scale_) * glm::translate(glm::mat4{}, translation_) * glm::mat_cast(rotation_);
        }

        /**
         * 
         *  Set matrix.
         * 
         *  @param m Transform matrix.
         * 
        */
        constexpr void set_matrix(const glm::mat4& m)
        {
            glm::mat4 row_m = glm::transpose(m);
            
            translation_ = {m[3][0], m[3][1], m[3][2]};

            auto scale_x = glm::length(glm::vec3{m[0][0], m[0][1], m[0][2]});
            auto scale_y = glm::length(glm::vec3{m[1][0], m[1][1], m[1][2]});
            auto scale_z = glm::length(glm::vec3{m[2][0], m[2][1], m[2][2]});

            scale_ = {scale_x, scale_y, scale_z};

            row_m[0][0] /= scale_x;
            row_m[0][1] /= scale_x;
            row_m[0][2] /= scale_x;
            row_m[1][0] /= scale_y;
            row_m[1][1] /= scale_y;
            row_m[1][2] /= scale_y;
            row_m[2][0] /= scale_z;
            row_m[2][1] /= scale_z;
            row_m[2][2] /= scale_z;
            row_m[3][0] = 0.0f;
            row_m[3][1] = 0.0f;
            row_m[3][2] = 0.0f;
            row_m[3][3] = 1.0f;

            rotation_ = glm::quat_cast(row_m);
        } 

        /**
         * 
         *  Get current translation.
         * 
         *  @return Translation.
         * 
        */
        constexpr glm::vec3 translation() const
        {
            return translation_;
        }

        /**
         * 
         *  Get current rotation.
         * 
         *  @return Rotation.
         * 
        */
        constexpr glm::quat rotation() const
        {
            return rotation_;
        }

        /**
         * 
         *  Get current scale.
         *
         *  @return Scale.
         *  
        */
        constexpr glm::vec3 scale() const
        {
            return scale_;
        }

        /**
         * 
         *  Set scale.
         * 
         *  @param scale Scale.
         * 
        */
        constexpr void set_scale(const glm::vec3& scale)
        {
            scale_ = scale;
        }

        /**
         * 
         *  Set rotation.
         * 
         *  @param rotation Rotation.
         * 
        */
        constexpr void set_rotation(const glm::quat& rotation)
        {
            rotation_ = rotation;
        }

        /**
         * 
         *  Set translation.
         * 
         *  @param translation Translation.
         * 
        */
        constexpr void set_translation(const glm::vec3& translation)
        {
            translation_ = translation;
        }

    private:
        glm::vec3 translation_;

        glm::quat rotation_;

        glm::vec3 scale_;
    };
}