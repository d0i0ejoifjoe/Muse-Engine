#pragma once

#include "utils/public/include_glm.h"
#include "utils/public/utils.h"

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
        DIRECTION,
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
        glm::vec4 color() const;

        /**
         * 
         *  Set light's color.
         * 
         *  @param color New color.
         * 
        */
        void set_color(const glm::vec4& color);

        /**
         * 
         *  Get projection matrix.
         * 
         *  @return Projection matrix.
         * 
        */
        virtual glm::mat4 projection() const = 0;

        /**
         * 
         *  Get view matrix.
         * 
         *  @return View matrix.
         * 
        */
        virtual glm::mat4 view() const = 0;

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
        virtual void set_direction(const glm::vec3& direction) = 0;

        /**
         * 
         *  Get direction of light.
         * 
         *  @return Direction vector.
         * 
        */
        virtual glm::vec3 direction() const = 0;

        /**
         * 
         *  Get position of light.
         * 
         *  @return Position vector.
         * 
        */
        virtual glm::vec3 position() const = 0;

        /**
         * 
         *  Set position.
         * 
         *  @param position New position.
         * 
        */
        virtual void set_position(const glm::vec3& position) = 0;

        /**
         * 
         *  Get attenuation coefficients.
         * 
         *  @return Const reference to attenuation coefficients. 
         * 
        */
        virtual Attenuation& attenuation() const = 0;

        /**
         * 
         *  Set attenuation.
         * 
         *  @param attenuation New attenuation.
         * 
        */
        virtual void set_attenuation(const Attenuation& attenuation) = 0;

    protected:
        /** Light intensity. */
        float intensity_;

        /** Light color. */
        glm::vec4 color_;
    };
}