#include "graphics/private/vertex_descriptor.h"
#include "graphics/public/animation.h"
#include "graphics/public/camera.h"
#include "graphics/public/mesh.h"
#include "graphics/public/mesh_manager.h"
#include "graphics/public/shader_system.h"
#include "graphics/public/texture_manager.h"
#include "graphics/public/window.h"
#include "log/public/logger.h"

#include <SDL.h>
#include <iostream>
#include <memory>

using KeyMap = std::unordered_map<std::int32_t, std::uint8_t>;

void handle_camera(KeyMap &map, muse::Camera &camera, float x, float y)
{
    static const auto speed = 2.0f;

    if (map[SDLK_w] == SDL_PRESSED)
    {
        camera.translate(camera.direction() * speed);
    }
    else if (map[SDLK_a] == SDL_PRESSED)
    {
        camera.translate(-camera.right() * speed);
    }
    else if (map[SDLK_s] == SDL_PRESSED)
    {
        camera.translate(-camera.direction() * speed);
    }
    else if (map[SDLK_d] == SDL_PRESSED)
    {
        camera.translate(camera.right() * speed);
    }

    static const auto sensitivity = 0.1f;

    camera.adjust_yaw(x * sensitivity);
    camera.adjust_pitch(y * sensitivity);
}

void a(const std::vector<muse::Animation> &animation)
{
    LOG_INFO(Callback, "Callback");
    LOG_INFO(Callback, "Animation size: {}", animation.size());
    return;
}

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    muse::Window window{};
    SDL_SetRelativeMouseMode(SDL_TRUE);

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT);
    // glFrontFace(GL_CW);

    muse::MeshManager manager{};

    std::vector<muse::Vertex> vertices{
        {{-0.5f, -0.5f, -10.0f}, {}, {1.0f, 0.0f, 0.0f, 1.0f}, {}, {}, {}},
        {{0.5f, -0.5f, -10.0f}, {}, {0.0f, 1.0f, 0.0f, 1.0f}, {}, {}, {}},
        {{0.0f, 0.5f, -10.0f}, {}, {0.0f, 0.0f, 1.0f, 1.0f}, {}, {}, {}},
    };

    std::vector<std::uint32_t> indices{
        0, 1, 2};

    muse::Skeleton skeleton{};

    // auto *loaded_mesh = manager.load("/home/sviatoslav/Documents/WizardusMaximus.fbx", false);
    auto *mesh = manager.create(vertices, indices, skeleton);

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

        uniform mat4 view;
        uniform mat4 proj;

        out vec4 col;

        void main()
        {
            col = aColor;
            gl_Position = proj * view * vec4(aPos, 1.0); 
        }
    )";

    std::string_view fragment_source = R"(
        #version 450 core
        #extension GL_ARB_bindless_texture : require

        out vec4 out_color;
        in vec4 col;

        void main()
        {
            out_color = col;
        }
    )";

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

    auto sys = std::make_unique<muse::ShaderSystem>(vertex_source, fragment_source, std::nullopt);
    auto playing = true;

    muse::Camera my_camera{muse::CameraType::PERSPECTIVE, std::addressof(window), 1000.0f};

    KeyMap keymap = {
        {SDLK_w, SDL_RELEASED},
        {SDLK_a, SDL_RELEASED},
        {SDLK_s, SDL_RELEASED},
        {SDLK_d, SDL_RELEASED},
        {SDLK_SPACE, SDL_RELEASED},
        {SDLK_LCTRL, SDL_RELEASED},
        {SDLK_ESCAPE, SDL_RELEASED}};

    sys->bind();
    mesh->bind();

    sys->add_uniform("view");
    sys->add_uniform("proj");

    my_camera.set_position({0.0f, 0.0f, 0.0f});

    SDL_Event e;

    float x = 0.0f;
    float y = 0.0f;

    while (playing)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_KEYDOWN)
            {
                keymap[e.key.keysym.sym] = SDL_PRESSED;
            }
            else if (e.type == SDL_KEYUP)
            {
                keymap[e.key.keysym.sym] = SDL_RELEASED;
            }
            else if (e.type == SDL_MOUSEMOTION)
            {
                x = static_cast<float>(e.motion.xrel);
                y = static_cast<float>(-e.motion.yrel);
            }
            else if (e.type == SDL_QUIT)
            {
                playing = false;
            }

            if (keymap[SDLK_ESCAPE] == SDL_PRESSED)
            {
                playing = false;
            }

            if (!playing)
            {
                break;
            }
        }

        handle_camera(keymap, my_camera, x, y);
        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sys->set_value("proj", my_camera.projection());
        sys->set_value("view", my_camera.view());
        // loaded_mesh->bind();
        // glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(loaded_mesh->element_count()), GL_UNSIGNED_INT, nullptr);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->element_count()), GL_UNSIGNED_INT, nullptr);
        SDL_GL_SwapWindow(window.handle());
    }

    return 0;
}