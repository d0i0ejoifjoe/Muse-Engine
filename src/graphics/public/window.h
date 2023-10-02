#pragma once

#include <memory>

/** Opaque defines */
typedef struct SDL_Window SDL_Window;
typedef void *SDL_GLContext;

namespace muse
{

/**
 *
 *  This window class it's a game window
 *  which has it's own OpenGL context and will
 *  be created in fullscreen mode.
 *
 */
class Window
{
  public:
    /**
     *
     *  Create a window.
     *
     */
    Window();

    /**
     *
     *  Destroy window.
     *
     */
    ~Window();

    /**
     *
     *  Get handle.
     *
     *  @return Handle.
     *
     */
    SDL_Window *handle() const;

  private:
    /** Handle */
    SDL_Window *handle_;

    /** Context */
    SDL_GLContext context_;
};

}