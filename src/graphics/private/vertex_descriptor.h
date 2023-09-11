#pragma once

#include "graphics/public/glad.h"
#include "graphics/public/buffer.h"

namespace muse
{
    /** Enum for every vertex element type */
    enum class VertexElementType : std::uint8_t
    {
        FLOAT2,
        FLOAT3,
        FLOAT4,
        INT4,
    };

    /**
     * 
     *  Vertex element structure.
     * 
    */
    struct VertexElement
    {
        /**
         * 
         *  Create vertex element.
         * 
         *  @param type Type of element.
         *  @param offset Offset in element array.
         * 
        */
        VertexElement(VertexElementType type, std::size_t offset)
            : type(type)
            , size(0)
            , count(0)
            , offset(offset)
        {
            switch(type)
            {
            case VertexElementType::FLOAT2: count = 2; size = sizeof(float) * count; break;
            case VertexElementType::FLOAT3: count = 3; size = sizeof(float) * count; break;
            case VertexElementType::FLOAT4: count = 4; size = sizeof(float) * count; break;
            case VertexElementType::INT4: count = 4; size = sizeof(std::uint32_t) * count; break;
            }
        }

        /** Type of element */
        VertexElementType type;

        /** Size of element */
        std::size_t size;

        /** Count of components it contains */
        std::size_t count;

        /** Offset in vertex element array */
        std::size_t offset;
    };

    /**
     * 
     *  Class that represents a vertex layout descriptor in buffer.
     * 
    */
    class VertexDescriptor
    {
    public:
        /** const iterator alias */
        using const_iterator = std::vector<VertexElement>::const_iterator;

        /**
         * 
         *  Create vertex descriptor.
         * 
         *  @param types All of element types.
         * 
        */
        explicit VertexDescriptor(const std::initializer_list<VertexElementType>& types)
            : size_(0)
            , elements_()
        {
            auto offset = 0u;

            for(const auto& type : types)
            {
                VertexElement element{type, offset};
                elements_.emplace(std::begin(elements_) + elements_.size(), element);
                offset += element.size;
            }
        }

        /**
         * 
         *  Get size of all elements in vertex descriptor array.
         *  
         *  @return Size.
         * 
        */
        std::size_t size() const
        {
            return size_;
        }

        /**
         * 
         *  Get pointer to beginning element of array.
         * 
         *  @return Pointer.
         * 
        */
        const_iterator begin()
        {
            return elements_.begin();
        }

        /**
         * 
         *  Get pointer to end element of array.
         * 
         *  @return Pointer.
         * 
        */
        const_iterator end()
        {
            return elements_.end();
        }

        /**
         * 
         *  Get const pointer to beginning element of array.
         * 
         *  @return Pointer.
         * 
        */
        const_iterator cbegin()
        {
            return elements_.cbegin();
        }
    
        /**
         * 
         *  Get const pointer to end element of array.
         * 
         *  @return Pointer.
         * 
        */
        const_iterator cend()
        {
            return elements_.cend();
        }

    private:
        /** Size */
        std::size_t size_;

        /** Array */
        std::vector<VertexElement> elements_;
    };

    /** Default vertex descriptor */
    inline VertexDescriptor DefaultVertexDescriptor{
        {VertexElementType::FLOAT3},
        {VertexElementType::FLOAT3},
        {VertexElementType::FLOAT4},
        {VertexElementType::FLOAT2},
        {VertexElementType::FLOAT3},
        {VertexElementType::FLOAT3},
        {VertexElementType::INT4},
        {VertexElementType::FLOAT4}
    };
}