#include "graphics/public/mesh_manager.h"
#include "graphics/public/mesh.h"
#include "log/public/logger.h"

#include <iostream>

void a(const std::vector<muse::Animation>& animation)
{
    LOG_INFO(Callback, "Callback");
    return;
}

int main()
{
    muse::MeshManager manager{};

    manager.load("/home/sviatoslav/Documents/WizardusMaximus.fbx", false, a);

    return 0;
}