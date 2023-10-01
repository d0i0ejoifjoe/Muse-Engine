#include "graphics/public/mesh_manager.h"
#include "graphics/public/mesh.h"
#include "graphics/public/animation.h"
#include "graphics/public/window.h"
#include "log/public/logger.h"

#include <iostream>
#include <SDL.h>

void a(const std::vector<muse::Animation>& animation)
{
    LOG_INFO(Callback, "Callback");
    LOG_INFO(Callback, "Animation size: {}", animation.size());
    return;
}

int main()
{
    muse::MeshManager manager{};

    SDL_Init(SDL_INIT_EVERYTHING);

    muse::Window window{};
    auto* mesh = manager.load("/home/sviatoslav/Documents/WizardusMaximus.fbx", false);
    mesh->bind();

    std::cin.get();

    return 0;
}