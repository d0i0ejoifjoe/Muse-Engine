#pragma once

#include "glad.h"

#include <cstddef>
#include <cstdint>

namespace muse
{
    class Sampler;

    /**
     * 
     *  Class that represents a cube map.
     * 
    */
    class CubeMap
    {
    public:
        /**
         * 
         *  Create cubemap.
         * 
         *  @param left_data Data of left side.
         *  @param right_data Data of right side.
         *  @param up_data Data of up side.
         *  @param down_data Data of down side.
         *  @param front_data Data of front side.
         *  @param back_data Data of back side.
         *  @param width Width of all sides.
         *  @param height Height of all sides.
         *  @param sampler Sampler to sample cubemap with.
         * 
        */
        CubeMap(std::byte* left_data,
                std::byte* right_data,
                std::byte* up_data,
                std::byte* down_data,
                std::byte* front_data,
                std::byte* back_data,
                std::uint32_t width,
                std::uint32_t height,
                Sampler* sampler);

        /**
         * 
         *  Destroy cubemap.
         * 
        */
        ~CubeMap();

        /**
         * 
         *  Get handle.
         * 
         *  @return Handle.
         * 
        */
        GLuint handle() const;

        /**
         * 
         *  Get bindless handle.
         * 
         *  @return Bindless handle.
         * 
        */
        GLuint64 bindless_handle() const;

        /**
         * 
         *  Get width of all sides.
         * 
         *  @return Width.
         * 
        */
        std::uint32_t width() const;

        /**
         * 
         *  Get height of all sides.
         * 
         *  @return Height.
         * 
        */
        std::uint32_t height() const;

    private:
        /** Width */
        std::uint32_t width_;

        /** Height */
        std::uint32_t height_;

        /** Handle */
        GLuint handle_;

        /** Bindless handle */
        GLuint64 bindless_handle_;
    };
}