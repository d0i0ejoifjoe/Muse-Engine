#pragma once

#include "glad.h"

#include <cstddef>
#include <vector>

namespace muse
{
    /**
     * 
     *  Class that represents GPU buffer.
     *  Used to store all kinds of data, like vertices, indices.
     *  and UBOs (Uniform buffer objects) is used to write to shaders
     *  like camera projection and view matrix, bone data, etc.
     * 
    */
    template<GLenum Target, GLenum Usage>
    class Buffer
    {
    public:
        /**
         * 
         *  Create buffer.
         * 
         *  @param size Initial size.
         * 
        */
        explicit Buffer(std::size_t size)
            : size_(size)
            , offset_(0)
            , handle_(0)
        {
            glGenBuffers(1, &handle_);
            glBindBuffer(Target, handle_);
            glBufferData(Target, size_, nullptr, Usage);
            glBindBuffer(Target, 0);
        }

        /**
         * 
         *  Create and bind buffer in certain range of binding point used only for UBOs, SSBOs.
         * 
         *  @param size Initial size.
         *  @param index Binding point.
         * 
        */
        Buffer(std::size_t size, GLuint index)
            : Buffer(size)
        {
            glBindBufferRange(Target, index, handle_, 0, size_);
        }

        /**
         * 
         *  Destroy buffer.
         * 
        */
        ~Buffer()
        {
            glDeleteBuffers(1, &handle_);
        }

        /** Deleted */
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;
        
        Buffer(Buffer&&) noexcept = default;
        Buffer& operator=(Buffer&&) noexcept = default;

        /**
         * 
         *  Get size of buffer.
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
         *  Get current offset in buffer.
         * 
         *  @return Offset.
         * 
        */
        std::size_t offset() const
        {
            return offset_;
        }

        /**
         * 
         *  Advance.
         * 
         *  @param offset How much to advance.
         * 
        */
        void advance(std::size_t offset)
        {
            offset_ += offset;
        }

        /**
         * 
         *  Get handle.
         * 
         *  @return Handle.
         * 
        */
        GLuint handle() const
        {
            return handle_;
        }

        /**
         * 
         *  Write vector to buffer.
         * 
         *  @param data Vector.
         * 
        */
        template<class T>
        void write(const std::vector<T>& data)
        {
            write(data.data(), sizeof(T) * data.size());
        }

        /**
         * 
         *  Write object to buffer.
         * 
         *  @param data Object.
         * 
        */
        template<class T>
        void write(const T& object)
        {
            write(std::addressof(object), sizeof(T));
        }

        /**
         * 
         *  Write to buffer.
         * 
         *  @param data Pointer to data.
         *  @param size Size of data (in bytes).
         * 
        */
        template<class T>
        void write(const T* data, std::size_t size)
        {
            if(offset_ + size > size_)
            {
                resize(std::max(size_ * 2, offset_ + size));
            }

            glBindBuffer(Target, handle_);

            glBufferSubData(Target, offset_, size, data);

            glBindBuffer(Target, 0);
        }

    private:
        /**
         * 
         *  Resize buffer.
         * 
         *  @param size New size for buffer.
         * 
        */
        void resize(std::size_t size)
        {
            // Preserve old size.
            std::size_t old_size = size_;
            size_ = size;

            // Create new buffer.
            GLuint new_handle = 0;
            glGenBuffers(1, &handle_);
            glBindBuffer(Target, handle_);
            glBufferData(Target, size_, nullptr, Usage);
            glBindBuffer(Target, 0);

            // Copy data to new buffer.
            glBindBuffer(GL_COPY_READ_BUFFER, handle_);

            glCopyBufferSubData(GL_COPY_READ_BUFFER, Target, 0, 0, old_size);

            // Delete old buffer and set current handle to new handle.
            glBindBuffer(GL_COPY_READ_BUFFER, 0);
            glDeleteBuffers(1, &handle_);

            handle_ = new_handle;            
        }

        /** Size */
        std::size_t size_;

        /** Offset */
        std::size_t offset_;

        /** Handle */
        GLuint handle_;
    };

    /** Aliases for different buffers */
    using VBO = Buffer<GL_ARRAY_BUFFER, GL_STATIC_DRAW>;
    using IBO = Buffer<GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW>;
    using UBO = Buffer<GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW>;
    using SSBO = Buffer<GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW>;
}