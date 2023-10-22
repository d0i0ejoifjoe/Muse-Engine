#include "graphics/public/window.h"

#include "graphics/public/glad.h"
#include "log/public/format.h"
#include "log/public/logger.h"

#include <SDL.h>
#include <cassert>

/**
 *
 *  Callback for opengl debug output system.
 *
 *  @param source Source of issue.
 *  @param type Type of issue.
 *  Unused.
 *  @param severity Severity of issue.
 *  Unused.
 *  @param message Message from opengl.
 *  Unused.
 *
 */
void gl_debug_output(GLenum source,
                     GLenum type,
                     GLenum id,
                     GLenum severity,
                     auto,
                     const char *message,
                     auto)
{
    std::string type_str = "";
    std::string source_str = "";
    std::string severity_str = "";

    if (id == 131185) // we skip this notification it just says that it gonna use video memory for buffer
    {
        return;
    }

    // Convert all of OpenGL Enums to strings
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            type_str = "error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            type_str = "deprecated behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            type_str = "undefined behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            type_str = "portability issue";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            type_str = "performance bottleneck";
            break;
        case GL_DEBUG_TYPE_MARKER:
            type_str = "marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            type_str = "push group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            type_str = "pop group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            type_str = "issue";
            break;
        default:
            type_str = "";
    }

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            source_str = "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            source_str = "Windowing system";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            source_str = "Shader compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            source_str = "Third party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            source_str = "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            source_str = "Other";
            break;
        default:
            source_str = "";
    }

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            severity_str = "Notification";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            severity_str = "Low";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            severity_str = "Medium";
            break;
        case GL_DEBUG_SEVERITY_HIGH:
            severity_str = "High";
            break;
        default:
            severity_str = "";
    }

    // Format and print full message to console.
    LOG_INFO(OpenGL, "\n*** OpenGL {} ***\n"
                     "Source: {}\n"
                     "Severity: {}\n"
                     "Message: {}\n",
             type_str, source_str, severity_str, message);
}

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

    if (!(flags & SDL_INIT_EVENTS))
        return false;

    if (!(flags & SDL_INIT_AUDIO))
        return false;

    if (!(flags & SDL_INIT_TIMER))
        return false;

    if (!(flags & SDL_INIT_VIDEO))
        return false;

    return true;
}

namespace muse
{

Window::Window()
    : handle_(nullptr)
    , context_(nullptr)
{
    // Check if SDL is initialized.
    assert(is_sdl_initialized() && "SDL is not initialized");

    // Setup all opengl attributes.
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

    // Create window.
    handle_ = SDL_CreateWindow(" ",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               1,
                               1,
                               SDL_WINDOW_FULLSCREEN_DESKTOP |
                                   SDL_WINDOW_OPENGL |
                                   SDL_WINDOW_ALWAYS_ON_TOP);

    // Create context.
    context_ = SDL_GL_CreateContext(handle_);
    SDL_GL_MakeCurrent(handle_, context_);

    // Load OpenGL Functions.
    auto success = gladLoadGLLoader(SDL_GL_GetProcAddress);
    assert(success != 0 && "failed to load opengl functions");

    // Enable multisampling and gamma correction
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Get context flags.
    GLint context_flags = GL_NONE;
    glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);

    // Check if context is debug
    if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        // If it is enable debug output and setup the callback
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(gl_debug_output, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
}

Window::~Window()
{
    // Delete context and window
    SDL_GL_DeleteContext(context_);
    SDL_DestroyWindow(handle_);
}

SDL_Window *Window::handle() const
{
    return handle_;
}

}