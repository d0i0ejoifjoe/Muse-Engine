#include "events/public/event_handler.h"
#include "graphics/private/vertex_descriptor.h"
#include "graphics/public/animation.h"
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
#include <iostream>
#include <memory>
#include <thread>

static constexpr auto speed = 250.0f;
static constexpr auto sensitivity = 0.1f;
float delta = 0.0f;
// FIXME: Make user not provide the width and height of monitor by theirself
static constexpr auto width = 1920u;
static constexpr auto height = 1080u;
static muse::Camera camera{muse::CameraType::PERSPECTIVE, width, height, 1000.0f};
static bool playing = true;
static std::unique_ptr<muse::Window> window;
static std::unique_ptr<muse::ShaderSystem> sys;
static std::unique_ptr<muse::TextureManager> tmanager;
static std::unique_ptr<muse::MaterialManager> mmanager;
static std::vector<muse::Mesh *> meshes{};
static muse::Mesh *mesh = nullptr;
static muse::Transform model_transform{};

void render()
{
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    sys->set_value("proj", camera.projection());
    sys->set_value("view", camera.view());
    for (const auto &mesh : meshes)
    {
        mesh->set_transform(model_transform);
        mesh->bind();
        sys->set_value("model", mesh->transform().matrix());
        sys->set_value("tex", mmanager->material(mesh->material_index())->albedo()->bindless_handle());
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->element_count()), GL_UNSIGNED_INT, nullptr);
    }
    mesh->set_transform(model_transform);
    sys->set_value("model", mesh->transform().matrix());
    mesh->bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->element_count()), GL_UNSIGNED_INT, nullptr);
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
            camera.translate(camera.direction() * speed_delta);
        }

        if (key[muse::Key::A] == muse::KeyState::DOWN)
        {
            camera.translate(camera.right() * speed_delta);
        }

        if (key[muse::Key::S] == muse::KeyState::DOWN)
        {
            camera.translate(-camera.direction() * speed_delta);
        }

        if (key[muse::Key::D] == muse::KeyState::DOWN)
        {
            camera.translate(-camera.right() * speed_delta);
        }

        if (key[muse::Key::LEFT_ARROW] == muse::KeyState::DOWN)
        {
            model_transform.set_scale(model_transform.scale() + glm::vec3{0.0f, 0.0f, 0.5f});
        }

        if (key[muse::Key::RIGHT_ARROW] == muse::KeyState::DOWN)
        {
            model_transform.set_scale(model_transform.scale() + glm::vec3{0.0f, 0.0f, -0.5f});
        }

        if (key[muse::Key::ESCAPE] == muse::KeyState::DOWN)
        {
            playing = false;
        }
    }

    if (e.type() == muse::EventType::MOUSE)
    {
        auto &mouse = e.get<muse::MouseEvent>();

        camera.adjust_yaw(static_cast<float>(mouse.delta_x) * sensitivity);
        camera.adjust_pitch(static_cast<float>(-mouse.delta_y) * sensitivity);

        camera.set_pitch(std::clamp(camera.pitch(), -89.0f, 89.0f));
    }

    return true;
}

bool handle_wheel(const muse::Event &e)
{
    if (e.type() == muse::EventType::WHEEL_SCROLL)
    {
        auto &wheel = e.get<muse::WheelScrollEvent>();

        model_transform.set_translation(model_transform.translation() + glm::vec3{0.0f, 0.0f, wheel.delta_y});
    }

    return false;
}

void loaded_animations(std::vector<muse::Animation> &&animations,
                       muse::Skeleton &&skeleton,
                       std::map<std::string, std::uint32_t, std::less<>> &&map)
{
    LOG_INFO(BoneCount, "Count: {}", map.size());

    for (const auto &animation : animations)
    {
        LOG_INFO(AnimationName, "Name: {}", animation.name());
    }
}

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    window = std::make_unique<muse::Window>();
    muse::EventHandler e_handler{};
    tmanager = std::make_unique<muse::TextureManager>();
    mmanager = std::make_unique<muse::MaterialManager>(tmanager.get());

    SDL_SetRelativeMouseMode(SDL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    muse::MeshManager manager{tmanager.get(), mmanager.get()};

    std::vector<muse::Vertex> vertices{
        {{-0.5f, -0.5f, -10.0f}, {}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {}, {}},
        {{0.5f, -0.5f, -10.0f}, {}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {}, {}},
        {{0.0f, 0.5f, -10.0f}, {}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.5f, 1.0f}, {}, {}},
    };

    std::vector<std::uint32_t> indices{
        0, 1, 2};

    muse::Skeleton skeleton{};

    /*std::vector<muse::Vertex> skybox_vertices{
        {{-1.0f, -1.0f, 1.0f}, {}, {}, {}, {}, {}},
        {{1.0f, -1.0f, 1.0f}, {}, {}, {}, {}, {}},
        {{1.0f, -1.0f, -1.0f}, {}, {}, {}, {}, {}},
        {{-1.0f, 1.0f, 1.0f}, {}, {}, {}, {}, {}},
        {{-1.0f, -1.0f, -1.0f}, {}, {}, {}, {}, {}},
        {{1.0f, 1.0f, 1.0f}, {}, {}, {}, {}, {}},
        {{1.0f, 1.0f, -1.0f}, {}, {}, {}, {}, {}},
        {{-1.0f, 1.0f, -1.0f}, {}, {}, {}, {}, {}},
    };

    std::vector<std::uint32_t> skybox_indices{
        1, 2, 6, 6, 5, 1, 0, 4, 7, 7, 3, 0,
        4, 5, 6, 6, 7, 4, 0, 3, 2, 2, 1, 0,
        0, 1, 5, 5, 4, 0, 3, 7, 6, 6, 2, 3};

    muse::Skeleton skybox_sk{};*/

    glm::quat rotation{glm::vec3{0.0f, 0.0f, 0.0f}};
    model_transform.set_rotation(rotation);

    meshes = manager.load("/home/sviatoslav/Documents/OtherModels/WizardusMaximus.fbx", loaded_animations, false);
    mesh = manager.create(vertices, indices, std::numeric_limits<std::uint32_t>::max());

    //  auto *skybox_mesh = manager.create(skybox_vertices, skybox_indices, skybox_sk);

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
        flat out ivec4 bone_ids;
        out vec4 weights;

        uniform mat4 view;
        uniform mat4 proj;
        uniform mat4 model;

        //uniform mat4 bones[100];

        void main()
        {
            //mat4 bone = bones[aBoneIDs[0]] * aWeights[0];
            //bone += bones[aBoneIDs[1]] * aWeights[1];
            //bone += bones[aBoneIDs[2]] * aWeights[2];
            //bone += bones[aBoneIDs[3]] * aWeights[3];

            gl_Position = proj * view * model * vec4(aPos, 1.0); 
            tex_coord = aTexCoord;
            col = aColor;
            bone_ids = aBoneIDs;
            weights = aWeights;
        }
    )";

    std::string_view fragment_source = R"(
        #version 450 core
        #extension GL_ARB_bindless_texture : require

        out vec4 out_color;

        in vec4 col;
        in vec2 tex_coord;
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

    /*std::string_view vertex_skybox = R"(
        #version 450 core
        #extension GL_ARB_bindless_texture : require

        layout(location = 0) in vec3 aPos;

        out vec3 tex_coords;

        uniform mat4 proj;
        uniform mat4 view;

        void main()
        {
            vec4 pos = proj * mat4(mat3(view)) * vec4(aPos, 1.0);
            gl_Position = pos.xyww;
            tex_coords = vec3(aPos.x, aPos.y, -aPos.z);
        }
    )";

    std::string_view fragment_skybox = R"(
        #version 450 core
        #extension GL_ARB_bindless_texture : require

        in vec3 tex_coords;
        out vec4 out_color;

        uniform samplerCube skybox;

        void main()
        {
            out_color = texture(skybox, tex_coords);
        }
    )";*/

    /**
    muse::VBO vbo{sizeof(muse::Vertex) * 3};
    muse::IBO ibo(sizeof(std::uint32_t) * 3);

    GLuint vao = 0u;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);

    std::vector<muse::Vertex> vertices{
        muse::Vertex{{-0.5f, -0.5f, 0.0f}, {}, {1.0f, 0.0f, 0.0f, 1.0f}, {}, {}, {}},
        muse::Vertex{{0.5f, -0.5f, 0.0f}, {}, {0.0f, 1.0f, 0.0f, 1.0f}, {}, {}, {}},
        muse::Vertex{{0.0f, 0.5f, 0.0f}, {}, {0.0f, 0.0f, 1.0f, 1.0f}, {}, {}, {}}};

    std::vector<std::uint32_t> indices{0, 1, 2};

    vbo.write(vertices);
    ibo.write(indices);

    glBindVertexArray(vao);

    auto size = muse::DefaultVertexDescriptor.size();
    auto index = 0u;

    glBindBuffer(GL_ARRAY_BUFFER, vbo.handle());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo.handle());

    for (const auto &element : muse::DefaultVertexDescriptor)
    {
        GLenum gl_type = GL_NONE;
        GLboolean normalize = GL_FALSE;
        switch (element.type)
        {
            case muse::VertexElementType::FLOAT2:
            case muse::VertexElementType::FLOAT3:
            case muse::VertexElementType::FLOAT4: gl_type = GL_FLOAT; break;
            case muse::VertexElementType::UINT4:
                gl_type = GL_UNSIGNED_INT;
                normalize = GL_TRUE;
                break;
        }

        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, static_cast<GLint>(element.count), gl_type, normalize, static_cast<GLsizei>(size), reinterpret_cast<void *>(element.offset));
        index++;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    */

    sys = std::make_unique<muse::ShaderSystem>(vertex_source, fragment_source, std::nullopt);
    // auto skybox_sys = std::make_unique<muse::ShaderSystem>(vertex_skybox, fragment_skybox, std::nullopt);

    LOG_INFO(RNG, "Integer: {}", muse::random_int<int>(1, 5));
    LOG_INFO(RNG, "Float: {}", muse::random_float<float>(0.1f, 0.9f));
    LOG_INFO(RNG, "Coin flip: {}", muse::flip_coin());

    sys->bind();

    sys->add_uniform("view");
    sys->add_uniform("proj");
    sys->add_uniform("model");
    sys->add_uniform("tex");

    e_handler.add_callback(handle_camera);
    e_handler.add_callback(handle_wheel);

    auto last = SDL_GetTicks();

    while (playing)
    {
        auto now = SDL_GetTicks();
        delta = static_cast<float>(now - last) / 1000.0f;
        last = now;

        e_handler.dispatch();

        render();
    }

    return 0;
}