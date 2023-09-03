#pragma once

#include <memory>

/** Opaque defines */
typedef struct SDL_Window SDL_Window;
typedef void* SDL_GLContext;

namespace muse
{
    /**
     * 
     *  Class that represents a GUI window.
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
        SDL_Window* handle() const;

    private:
        /** Handle */
        SDL_Window* handle_;

        /** Context */
        SDL_GLContext context_;
    };
}