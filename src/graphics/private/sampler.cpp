#include "graphics/public/sampler.h"

GLenum to_opengl(muse::SamplerAddressMode mode)
{
    switch (mode)
    {
        case muse::SamplerAddressMode::MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
            break;
        case muse::SamplerAddressMode::REPEAT:
            return GL_REPEAT;
            break;
        case muse::SamplerAddressMode::CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
            break;
        case muse::SamplerAddressMode::CLAMP_TO_BORDER:
            return GL_CLAMP_TO_BORDER;
            break;
    }

    return GL_NONE;
}

GLenum to_opengl(muse::SamplerFilter filter)
{
    switch (filter)
    {
        case muse::SamplerFilter::NEAREST:
            return GL_NEAREST;
            break;
        case muse::SamplerFilter::LINEAR:
            return GL_LINEAR;
            break;
    }

    return GL_NONE;
}

namespace muse
{

Sampler::Sampler(const SamplerSpecification &spec)
    : handle_(0)
    , specification_(spec)
{
    glGenSamplers(1, &handle_);

    glSamplerParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, to_opengl(specification_.s_mode));
    glSamplerParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, to_opengl(specification_.t_mode));
    glSamplerParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, to_opengl(specification_.r_mode));
    glSamplerParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &specification_.border_color[0]);
    glSamplerParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, to_opengl(specification_.mag_filter));

    if (specification_.use_mipmaps)
    {
        if (specification_.min_filter == SamplerFilter::LINEAR && specification_.mipmap_filter == SamplerFilter::LINEAR)
        {
            glSamplerParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }
        else if (specification_.min_filter == SamplerFilter::LINEAR &&
                 specification_.mipmap_filter == SamplerFilter::NEAREST)
        {
            glSamplerParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        }
        else if (specification_.min_filter == SamplerFilter::NEAREST &&
                 specification_.mipmap_filter == SamplerFilter::NEAREST)
        {
            glSamplerParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        }
        else if (specification_.min_filter == SamplerFilter::NEAREST &&
                 specification_.mipmap_filter == SamplerFilter::LINEAR)
        {
            glSamplerParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }
    }
    else
    {
        glSamplerParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, to_opengl(specification_.min_filter));
    }
}

Sampler::~Sampler()
{
    glDeleteSamplers(1, &handle_);
}

GLuint Sampler::handle() const
{
    return handle_;
}

const SamplerSpecification &Sampler::specification() const
{
    return specification_;
}

}