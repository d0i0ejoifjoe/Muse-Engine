#include "graphics/public/window.h"

#include <SDL.h>
#include <cassert>

/**
 * 
 *  Check if SDL was initialized.
 * 
 *  @return True if all needed systems initialized properly otherwise false.
 * 
*/
bool is_sdl_initialized()
{
    Uint32 flags = SDL_WasInit(0);
    
    if(!(flags & SDL_INIT_EVENTS))
        return false;

    if(!(flags & SDL_INIT_AUDIO))
        return false;

    if(!(flags & SDL_INIT_TIMER))
        return false;

    if(!(flags & SDL_INIT_VIDEO))
        return false;

    return true;
}

namespace muse
{
    Window::Window()
        : handle_(nullptr)
        , context_(nullptr)
    {
        assert(is_sdl_initialized() && "SDL is not initialized");

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, SDL_TRUE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG | SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        handle_ = SDL_CreateWindow(" ", 
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   1,
                                   1,
                                   SDL_WINDOW_FULLSCREEN_DESKTOP |
                                   SDL_WINDOW_OPENGL |
                                   SDL_WINDOW_ALWAYS_ON_TOP);
        
        context_ = SDL_GL_CreateContext(handle_);
        SDL_GL_MakeCurrent(handle_, context_);
    }

    Window::~Window()
    {
        SDL_GL_DeleteContext(context_);
        SDL_DestroyWindow(handle_);
    }

    SDL_Window* Window::handle() const
    {
        return handle_;
    }

}