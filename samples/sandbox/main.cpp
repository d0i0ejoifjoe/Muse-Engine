#include "events/public/event_handler.h"
#include "graphics/private/vertex_descriptor.h"
#include "graphics/public/animation/animation.h"
#include "graphics/public/camera.h"
#include "graphics/public/material.h"
#include "graphics/public/material_manager.h"
#include "graphics/public/mesh.h"
#include "graphics/public/mesh_manager.h"
#include "graphics/public/shader_system.h"
#include "graphics/public/texture_manager.h"
#include "graphics/public/window.h"
#include "log/public/logger.h"
#include "utils/public/utils.h"

#include <SDL.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>

/* Constants */
static constexpr auto speed = 250.0f;
static constexpr auto sensitivity = 0.1f;
float delta = 0.0f;
// FIXME: Make user not provide the width and height of monitor by theirself
static constexpr auto width = 1920u;
static constexpr auto height = 1080u;

static muse::Camera perspective_camera{muse::CameraType::INFINITE_PERSPECTIVE, width, height, 10000.0f};
static muse::Camera ortho_camera{muse::CameraType::ORTHOGRAPHIC, 100, 100, 10000.0f};
static muse::Camera *primary = nullptr;
static bool playing = true;
static std::unique_ptr<muse::Window> window;
static std::unique_ptr<muse::ShaderSystem> sys;
static std::unique_ptr<muse::EventHandler> e_handler;
static std::unique_ptr<muse::TextureManager> tmanager;
static std::unique_ptr<muse::MaterialManager> mmanager;
static std::vector<muse::Mesh *> meshes1{};
static std::vector<muse::Mesh *> meshes2{};
static muse::Texture *zombie_diffuse = nullptr;
static muse::Texture *backpack_diffuse = nullptr;

void render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    sys->bind();
    sys->set_value("proj", primary->projection());
    sys->set_value("view", primary->view());
    sys->set_value("tex", zombie_diffuse->bindless_handle());
    for (const auto &mesh : meshes1)
    {
        mesh->bind();
        sys->set_value("model", mesh->transform().matrix() * muse::Matrix4::translate(muse::Vector3{0.0f, 0.0f, -10.0f}));
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->element_count()), GL_UNSIGNED_INT, nullptr);
    }

    sys->set_value("tex", backpack_diffuse->bindless_handle());
    for (const auto &mesh : meshes2)
    {
        mesh->bind();
        sys->set_value("model", mesh->transform().matrix());
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->element_count()), GL_UNSIGNED_INT, nullptr);
    }

    SDL_GL_SwapWindow(window->handle());
}

bool handle_camera(const muse::Event &e)
{
    if (e.type() == muse::EventType::KEYBOARD)
    {
        auto speed_delta = speed * delta;

        auto &key = e.get<muse::KeyboardEvent>();

        if (key[muse::Key::W] == muse::KeyState::DOWN)
        {
            primary->translate(primary->direction() * speed_delta);
        }

        if (key[muse::Key::A] == muse::KeyState::DOWN)
        {
            primary->translate(primary->right() * speed_delta);
        }

        if (key[muse::Key::S] == muse::KeyState::DOWN)
        {
            primary->translate(-primary->direction() * speed_delta);
        }

        if (key[muse::Key::D] == muse::KeyState::DOWN)
        {
            primary->translate(-primary->right() * speed_delta);
        }

        if (key[muse::Key::SPACE] == muse::KeyState::DOWN)
        {
            primary->translate(primary->up() * speed_delta);
        }

        if (key[muse::Key::LCTRL] == muse::KeyState::DOWN)
        {
            primary->translate(-primary->up() * speed_delta);
        }

        if (key[muse::Key::ESCAPE] == muse::KeyState::DOWN)
        {
            playing = false;
        }

        if (key[muse::Key::Q] == muse::KeyState::DOWN)
        {
            static bool flip = false;

            if (flip)
            {
                primary = std::addressof(perspective_camera);
                flip = false;
            }
            else
            {
                primary = std::addressof(ortho_camera);
                flip = true;
            }
        }
    }

    if (e.type() == muse::EventType::MOUSE)
    {
        auto &mouse = e.get<muse::MouseEvent>();

        primary->adjust_yaw(static_cast<float>(mouse.delta_x) * sensitivity);
        primary->adjust_pitch(static_cast<float>(-mouse.delta_y) * sensitivity);

        primary->set_pitch(std::clamp(primary->pitch(), -89.0f, 89.0f));
    }

    return true;
}

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    window = std::make_unique<muse::Window>();
    e_handler = std::make_unique<muse::EventHandler>();
    tmanager = std::make_unique<muse::TextureManager>();
    mmanager = std::make_unique<muse::MaterialManager>(tmanager.get());

    SDL_SetRelativeMouseMode(SDL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    muse::MeshManager manager{tmanager.get(), mmanager.get()};

    primary = std::addressof(perspective_camera);

    meshes1 = manager.load("/home/sviatoslav/Documents/OtherModels/Zombie.fbx", nullptr, false, true);
    meshes2 = manager.load("/home/sviatoslav/Documents/BackpackMapTest/backpack.obj", nullptr, false, false);
    LOG_INFO(WE, "WE ARE HERE");

    std::string_view vertex_source = R"(
        #version 450 core
        #extension GL_ARB_bindless_texture : require
        
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aNormal;
        layout(location = 2) in vec4 aColor;
        layout(location = 3) in vec2 aTexCoord;
        layout(location = 4) in vec3 aTangent;
        layout(location = 5) in vec3 aBitangent;
        layout(location = 6) in ivec4 aBoneIDs;
        layout(location = 7) in vec4 aWeights;

        out vec4 col; 
        out vec2 tex_coord;
        out flat ivec4 bone_ids;
        out vec4 weights;
        out vec3 frag_pos;
        out vec3 normal;

        uniform mat4 view;
        uniform mat4 proj;
        uniform mat4 model;

        void main()
        {
            gl_Position = proj * view * model * vec4(aPos, 1.0f); 
            frag_pos = vec3(model * vec4(aPos, 1.0f));
            tex_coord = aTexCoord;
            col = aColor;
            bone_ids = aBoneIDs;
            weights = aWeights;
            normal = aNormal;
        }
    )";

    std::string_view fragment_source = R"(
        #version 450 core
        #extension GL_ARB_bindless_texture : require

        out vec4 out_color;

        in vec4 col;
        in vec2 tex_coord;
        in vec3 frag_pos;
        in vec3 normal;
        uniform sampler2D tex;

        flat in ivec4 bone_ids;
        in vec4 weights;

        void main()
        {
            if(false)
            {
                bool found = false;

                for(int i = 0; i < 4; i++)
                {
                    if(weights[i] >= 0.7)
                    {
                        out_color = vec4(1.0, 0.0, 0.0, 0.0) * weights[i];
                        found = true;
                        break;
                    }
                    else if(weights[i] >= 0.4 && weights[i] <= 0.6)
                    {
                        out_color = vec4(0.0, 1.0, 0.0, 0.0) * weights[i];
                        found = true;
                        break;
                    }
                    else if(weights[i] >= 0.1)
                    {
                        out_color = vec4(1.0, 1.0, 0.0, 0.0) * weights[i];
                        found = true;
                        break;
                    }
                    else
                    {
                        found = false;
                        break;
                    }
                }

                if(!found)
                {
                    out_color = vec4(0.0, 0.0, 1.0, 1.0);
                }
            }
            else
            {
               out_color = texture(tex, tex_coord);
            }
        }
    )";

    sys = std::make_unique<muse::ShaderSystem>(vertex_source, fragment_source, std::nullopt);
    sys->bind();

    sys->add_uniform("view");
    sys->add_uniform("proj");
    sys->add_uniform("model");
    sys->add_uniform("tex");

    zombie_diffuse = tmanager->load("/home/sviatoslav/Documents/OtherModels/ZombieTexture.png", nullptr, muse::TextureFormat::SRGB_ALPHA, false);
    backpack_diffuse = tmanager->load("/home/sviatoslav/Documents/BackpackMapTest/diffuse.jpg", nullptr, muse::TextureFormat::SRGB_ALPHA, false);

    e_handler->add_callback(handle_camera);

    auto last = SDL_GetTicks();

    while (playing)
    {
        auto now = SDL_GetTicks();
        delta = static_cast<float>(now - last) / 1000.0f;
        last = now;

        e_handler->dispatch();

        render();
    }

    return 0;
}