#pragma once

#include "cube_map.h"
#include "texture.h"

#include <type_traits>

namespace muse
{

template <class T>
concept IsTexture = std::is_same_v<T, Texture>;

template <class T>
concept IsCubeMap = std::is_same_v<T, CubeMap>;

template <class T>
constexpr bool is_texture = IsTexture<T>;

template <class T>
constexpr bool is_cubemap = IsCubeMap<T>;

template <class T>
concept IsAttachment = std::is_same_v<T, CubeMap> || std::is_same_v<T, Texture>;

template <IsAttachment ColorAttachment,
          IsAttachment PositionAttachment,
          IsAttachment NormalAttachment,
          IsAttachment DepthAttachment,
          IsAttachment RoughnessAttachment,
          IsAttachment MetalnessAttachment,
          IsAttachment AmbientOcculusionAttachment>
class Framebuffer
{
  public:
    Framebuffer(ColorAttachment *color_attachment,
                PositionAttachment *position_attachment,
                NormalAttachment *normal_attachment,
                DepthAttachment *depth_attachment,
                RoughnessAttachment *roughness_attachment,
                MetalnessAttachment *metalness_attachment,
                AmbientOcculusionAttachment *ao_attachment)
        : handle_(0)
        , color_attachment_(color_attachment)
        , position_attachment_(position_attachment)
        , normal_attachment_(normal_attachment)
        , depth_attachment_(depth_attachment)
        , roughness_attachment_(roughness_attachment)
        , metalness_attachment_(metalness_attachment)
        , ao_attachment_(ao_attachment)
    {
        // Create framebuffer
        glGenFramebuffers(1, &handle_);
        glBindFramebuffer(GL_FRAMEBUFFER, handle_);

        std::array<GLenum, 6> attachments = {GL_NONE, GL_NONE, GL_NONE, GL_NONE, GL_NONE, GL_NONE};

        if (color_attachment_)
        {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_attachment_->handle(), 0);
            attachments[0] = GL_COLOR_ATTACHMENT0;
        }

        if (depth_attachment_)
        {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_attachment_->handle(), 0);
        }

        if (position_attachment_)
        {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, position_attachment_->handle(), 0);
            attachments[1] = GL_COLOR_ATTACHMENT1;
        }

        if (normal_attachment_)
        {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, normal_attachment_->handle(), 0);
            attachments[2] = GL_COLOR_ATTACHMENT2;
        }

        if (roughness_attachment_)
        {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, roughness_attachment_->handle(), 0);
            attachments[3] = GL_COLOR_ATTACHMENT3;
        }

        if (metalness_attachment_)
        {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, metalness_attachment_->handle(), 0);
            attachments[4] = GL_COLOR_ATTACHMENT4;
        }

        if (ao_attachment_)
        {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, ao_attachment_->handle(), 0);
            attachments[5] = GL_COLOR_ATTACHMENT5;
        }

        LOG_INFO(Attachments, "Number of attachments: {}", attachments.size());

        // Check if any of textures are attached to framebuffer
        if (std::any_of(std::cbegin(attachments), std::cend(attachments), [&](GLenum attachment)
                        {
                            return attachment != GL_NONE;
                        }))
        {
            // Specify to what attachments we gonna draw to (depth is set by default).
            glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());
        }

        // Check if framebuffer is completed
        auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::string error_string = "";
            switch (status)
            {
                case 36054:
                    error_string = R"(Not all framebuffer attachment points are attachment complete.
											  This can mean one of three things, first that at least one attachment has it's texture object no longer in existence,
											  width/height of texture image is zero or the color attachment is non-color-renderable
											  or depth attachment is non-depth-renderable)";
                    break;
                case 36057: error_string = R"(Not all attachments have same width/height)"; break;
                case 36055: error_string = R"(No textures attached to framebuffer)"; break;
                case 36061: error_string = R"(The combination of internal formats of attached textures violates implementation dependent set of restrictions)"; break;
            }

            LOG_ERROR(RenderTarget, "Failed to create render target, framebuffer is not complete\nStatus: {}\nError string: {}", status, error_string);
        }
    }

    ~Framebuffer()
    {
        glDeleteFramebuffers(1, &handle_);
    }

    void bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, handle_);
    }

    GLuint handle() const
    {
        return handle_;
    }

    ColorAttachment *color_attachment() const
    {
        return color_attachment_;
    }

    PositionAttachment *position_attachment() const
    {
        return position_attachment_;
    }

    NormalAttachment *normal_attachment() const
    {
        return normal_attachment_;
    }

    DepthAttachment *depth_attachment() const
    {
        return depth_attachment_;
    }

    RoughnessAttachment *roughness_attachment() const
    {
        return roughness_attachment_;
    }

    MetalnessAttachment *metalness_attachment() const
    {
        return metalness_attachment_;
    }

    AmbientOcculusionAttachment *ao_attachment() const
    {
        return ao_attachment_;
    }

  private:
    GLuint handle_;

    ColorAttachment *color_attachment_;

    PositionAttachment *position_attachment_;

    NormalAttachment *normal_attachment_;

    DepthAttachment *depth_attachment_;

    RoughnessAttachment *roughness_attachment_;

    MetalnessAttachment *metalness_attachment_;

    AmbientOcculusionAttachment *ao_attachment_;
};

template <class TextureType>
using RenderTarget = Framebuffer<TextureType, TextureType, TextureType, TextureType, TextureType, TextureType, TextureType>;

using GeneralRenderTarget = RenderTarget<Texture>;
using CubeMapRenderTarget = RenderTarget<CubeMap>;

}