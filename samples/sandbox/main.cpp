#include "events/public/event_handler.h"
#include "graphics/private/vertex_descriptor.h"
#include "graphics/public/animation/animation.h"
#include "graphics/public/camera.h"
#include "graphics/public/material.h"
#include "graphics/public/material_manager.h"
#include "graphics/public/mesh.h"
#include "graphics/public/mesh_manager.h"
#include "graphics/public/render_target.h"
#include "graphics/public/shader_library.h"
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
static muse::ShaderSystem *screen_sys;
static muse::ShaderSystem *directions_sys;
static muse::ShaderSystem *skybox_sys;
static muse::ShaderSystem *default_sys;
static muse::ShaderSystem *pbr_sys;
static muse::ShaderSystem *ball_pbr_sys;
static std::unique_ptr<muse::EventHandler> e_handler;
static std::unique_ptr<muse::TextureManager> tmanager;
static std::unique_ptr<muse::MaterialManager> mmanager;
static std::unique_ptr<muse::MeshManager> mesh_manager;
static std::vector<muse::Mesh *> meshes1{};
static std::vector<muse::Mesh *> meshes2{};
static std::vector<muse::Mesh *> meshes3{};
static muse::Mesh *directions_mesh = nullptr;
static muse::Mesh *skybox_mesh = nullptr;
static muse::Mesh *screen_mesh = nullptr;
static muse::Texture *zombie_diffuse = nullptr;
static muse::Texture *screen_albedo = nullptr;
static muse::Texture *screen_depth = nullptr;
static muse::Texture *screen_normal = nullptr;
static muse::Texture *screen_position = nullptr;
static muse::Texture *screen_roughness = nullptr;
static muse::Texture *screen_metallic = nullptr;
static muse::Texture *screen_ao = nullptr;
static muse::Texture *hdr_texture = nullptr;
static std::unique_ptr<muse::GeneralRenderTarget> render_target;
static muse::CubeMap *skybox_cubemap = nullptr;
static muse::Sampler *skybox_sampler = nullptr;
static muse::Sampler *render_target_sampler = nullptr;
static muse::FileManager *fmanager = nullptr;
static muse::Material *pbr_material = nullptr;

// FIXME: Deffered rendering doesn't work with PBR well here it doesn't
// First artifact that the skybox has effects like in portal when you look out of bounds
// Second artifact: Nothing is written into textures to then proceed with calculations in screen shader
// Maybe something is corrupt but you need to render something first and then fix the skybox artifact and it looks like the color is more tone mapped
// maybe i really dont know Good luck fixing it
void render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, render_target->handle());
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    default_sys->bind();
    default_sys->set_value("proj", primary->projection());
    default_sys->set_value("view", primary->view());
    default_sys->set_value("tex", zombie_diffuse->bindless_handle());
    for (const auto &mesh : meshes1)
    {
        mesh->bind();
        default_sys->set_value("model", mesh->transform().matrix() * muse::Matrix4::translate(muse::Vector3{0.0f, 0.0f, 10.0f}));
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->element_count()), GL_UNSIGNED_INT, nullptr);
    }

    pbr_sys->bind();
    muse::UBO camera_ubo{(sizeof(muse::Matrix4) * 2) + sizeof(muse::Vector3), 0};
    camera_ubo.write(muse::Matrix4::transpose(primary->projection()));
    camera_ubo.write(muse::Matrix4::transpose(primary->view()));
    camera_ubo.write(primary->position());

    muse::UBO material_textures{sizeof(GLuint64) * 5, 3};

    material_textures.write(pbr_material->albedo()->bindless_handle());
    material_textures.write(pbr_material->normal()->bindless_handle());
    material_textures.write(pbr_material->ao()->bindless_handle());
    material_textures.write(pbr_material->roughness()->bindless_handle());
    material_textures.write(pbr_material->metallic()->bindless_handle());

    // FIXME: Cant write to the program properly
    // Error: Target buffer must be bound and target range must not be mapped.
    // Second error: Target buffer must be bound and not overlapped with mapping range.

    for (const auto &mesh : meshes2)
    {
        mesh->bind();
        muse::UBO model_data{sizeof(muse::Matrix4) * 2, 1};
        auto model_transform = mesh->transform().matrix();
        auto normal_matrix = model_transform;
        normal_matrix[3] = 0.0f;
        normal_matrix[7] = 0.0f;
        normal_matrix[11] = 0.0f;

        normal_matrix = muse::Matrix4::transpose(muse::Matrix4::inverse(model_transform));
        model_data.write(muse::Matrix4::transpose(model_transform));
        model_data.write(normal_matrix);

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->element_count()), GL_UNSIGNED_INT, nullptr);
    }

    ball_pbr_sys->bind();
    ball_pbr_sys->set_value("proj", primary->projection());
    ball_pbr_sys->set_value("view", primary->view());
    ball_pbr_sys->set_value("view_pos", primary->position());
    ball_pbr_sys->set_value("albedo", muse::Vector3{1.0f, 0.0f, 0.0f});
    ball_pbr_sys->set_value("roughness", 0.0f);
    ball_pbr_sys->set_value("metallic", 0.0f);
    ball_pbr_sys->set_value("ao", 1.0f);
    for (const auto &mesh : meshes3)
    {
        mesh->bind();
        ball_pbr_sys->set_value("model", mesh->transform().matrix() * muse::Matrix4::translate(muse::Vector3{0.0f, 0.0f, 5.0f}));
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->element_count()), GL_UNSIGNED_INT, nullptr);
    }

    directions_sys->bind();
    directions_sys->set_value("proj", primary->projection());
    directions_sys->set_value("view", primary->view());

    directions_mesh->bind();
    glDrawElements(GL_LINES, static_cast<GLsizei>(directions_mesh->element_count()), GL_UNSIGNED_INT, nullptr);

    skybox_sys->bind();
    skybox_sys->set_value("proj", primary->projection());
    skybox_sys->set_value("view", primary->view());

    skybox_mesh->bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(skybox_mesh->element_count()), GL_UNSIGNED_INT, nullptr);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT);

    screen_sys->bind();
    screen_mesh->bind();

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(screen_mesh->element_count()), GL_UNSIGNED_INT, nullptr);

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

    fmanager = std::addressof(muse::FileManager::instance());
    fmanager->set_root_directory("/home/sviatoslav/Documents");

    window = std::make_unique<muse::Window>();
    e_handler = std::make_unique<muse::EventHandler>();
    tmanager = std::make_unique<muse::TextureManager>(fmanager);
    mmanager = std::make_unique<muse::MaterialManager>(tmanager.get());
    mesh_manager = std::make_unique<muse::MeshManager>(fmanager, tmanager.get(), mmanager.get());

    pbr_material = mmanager->load(muse::MaterialPaths{
                                      .albedo = "Assets/BackpackMapTest/1001_albedo.jpg",
                                      .normal = "Assets/BackpackMapTest/1001_normal.png",
                                      .metallic = "Assets/BackpackMapTest/1001_metallic.jpg",
                                      .roughness = "Assets/BackpackMapTest/1001_roughness.jpg",
                                      .ao = "Assets/BackpackMapTest/1001_AO.jpg",
                                  },
                                  "pbr_material");

    SDL_SetRelativeMouseMode(SDL_TRUE);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    primary = std::addressof(perspective_camera);

    meshes1 = mesh_manager->load("Assets/OtherModels/Zombie.fbx", nullptr, false, true);
    meshes2 = mesh_manager->load("Assets/BackpackMapTest/backpack.obj", nullptr, false);
    meshes3 = mesh_manager->load("Assets/OtherModels/sphere.obj", nullptr, false);

    auto shader_lib = std::make_unique<muse::ShaderLibrary>();

    // FIXME: Make that we just have to write the name of file to include it in .glsl file so something like this #include "some_func.glsl"
    shader_lib->set_root_directory("/home/sviatoslav/Documents/Assets/Shaders");

    shader_lib->load_shader("skybox.glsl", "skybox_shader");
    skybox_sys = shader_lib->compile_shader("skybox_shader", "skybox_system");
    shader_lib->load_shader("screen.glsl", "screen_shader");
    screen_sys = shader_lib->compile_shader("screen_shader", "screen_system");
    shader_lib->load_shader("directions.glsl", "directions_shader");
    directions_sys = shader_lib->compile_shader("directions_shader", "directions_system");
    shader_lib->load_shader("default.glsl", "default_shader");
    default_sys = shader_lib->compile_shader("default_shader", "default_system");
    shader_lib->load_shader("geometry.glsl", "geometry_shader");
    pbr_sys = shader_lib->compile_shader("geometry_shader", "geometry_system");
    shader_lib->load_shader("ball_pbr.glsl", "ball_pbr_shader");
    ball_pbr_sys = shader_lib->compile_shader("ball_pbr_shader", "ball_pbr_system");

    ball_pbr_sys->add_uniform("proj");
    ball_pbr_sys->add_uniform("view");
    ball_pbr_sys->add_uniform("model");
    ball_pbr_sys->add_uniform("view_pos");
    ball_pbr_sys->add_uniform("albedo");
    ball_pbr_sys->add_uniform("roughness");
    ball_pbr_sys->add_uniform("metallic");
    ball_pbr_sys->add_uniform("ao");

    pbr_sys->add_uniform("proj");
    pbr_sys->add_uniform("view");
    pbr_sys->add_uniform("model");
    pbr_sys->add_uniform("albedo_tex");
    pbr_sys->add_uniform("normal_tex");
    pbr_sys->add_uniform("roughness_tex");
    pbr_sys->add_uniform("metallic_tex");
    pbr_sys->add_uniform("ao_tex");

    directions_sys->add_uniform("proj");
    directions_sys->add_uniform("view");

    skybox_sys->add_uniform("view");
    skybox_sys->add_uniform("proj");
    skybox_sys->add_uniform("skybox");

    screen_sys->add_uniform("color_target");
    screen_sys->add_uniform("normal_target");
    screen_sys->add_uniform("position_target");
    screen_sys->add_uniform("roughness_target");
    screen_sys->add_uniform("metalness_target");
    screen_sys->add_uniform("ao_target");
    screen_sys->add_uniform("view_pos");

    default_sys->add_uniform("proj");
    default_sys->add_uniform("view");
    default_sys->add_uniform("model");
    default_sys->add_uniform("tex");

    std::vector<muse::Vertex>
        vertices = {
            {{0.0f, 0.0f, 0.0f}, {}, {1.0f, 0.0f, 0.0f, 1.0f}, {}, {}, {}},
            {{10.0f, 0.0f, 0.0f}, {}, {1.0f, 0.0f, 0.0f, 1.0f}, {}, {}, {}},
            {{0.0f, 0.0f, 0.0f}, {}, {0.0f, 1.0f, 0.0f, 1.0f}, {}, {}, {}},
            {{0.0f, 10.0f, 0.0f}, {}, {0.0f, 1.0f, 0.0f, 1.0f}, {}, {}, {}},
            {{0.0f, 0.0f, 0.0f}, {}, {0.0f, 0.0f, 1.0f, 1.0f}, {}, {}, {}},
            {{0.0f, 0.0f, 10.0f}, {}, {0.0f, 0.0f, 1.0f, 1.0f}, {}, {}, {}}};

    std::vector<std::uint32_t> indices = {0, 1, 2, 3, 4, 5};

    std::vector<muse::Vertex> skybox_vertices = {
        {{-1.0f, -1.0f, 1.0f}, {}, {}, {}, {}, {}},
        {{1.0f, -1.0f, 1.0f}, {}, {}, {}, {}, {}},
        {{1.0f, -1.0f, -1.0f}, {}, {}, {}, {}, {}},
        {{-1.0f, -1.0f, -1.0f}, {}, {}, {}, {}, {}},
        {{-1.0f, 1.0f, 1.0f}, {}, {}, {}, {}, {}},
        {{1.0f, 1.0f, 1.0f}, {}, {}, {}, {}, {}},
        {{1.0f, 1.0f, -1.0f}, {}, {}, {}, {}, {}},
        {{-1.0f, 1.0f, -1.0f}, {}, {}, {}, {}, {}},
    };

    std::vector<std::uint32_t> skybox_indices = {1, 2, 6,
                                                 6, 5, 1,
                                                 0, 4, 7,
                                                 7, 3, 0,
                                                 4, 5, 6,
                                                 6, 7, 4,
                                                 0, 3, 2,
                                                 2, 1, 0,
                                                 0, 1, 5,
                                                 5, 4, 0,
                                                 3, 7, 6,
                                                 6, 2, 3};

    std::vector<muse::Vertex> screen_quad_vertices = {
        {{-1.0f, 1.0f, 0.0f}, {}, {}, {0.0f, 1.0f}, {}, {}},  //     0-----------1        +1
        {{1.0f, 1.0f, 0.0f}, {}, {}, {1.0f, 1.0f}, {}, {}},   //     |           |        |
        {{1.0f, -1.0f, 0.0f}, {}, {}, {1.0f, 0.0f}, {}, {}},  //    |           |  -1 <- 0 -> +1
        {{-1.0f, -1.0f, 0.0f}, {}, {}, {0.0f, 0.0f}, {}, {}}, //    2-----------3        |
                                                              //                         -1
    };

    std::vector<std::uint32_t> screen_quad_indices = {0, 1, 2, 2, 3, 0};

    screen_mesh = mesh_manager->create(screen_quad_vertices, screen_quad_indices);
    directions_mesh = mesh_manager->create(vertices, indices);
    skybox_mesh = mesh_manager->create(skybox_vertices, skybox_indices);

    // FIXME: Fix thing with textures and cubemap, check if the problem is in samplers or textures,or cubemaps or GLSL sampler
    // Some useful links: https://stackoverflow.com/questions/56527117/opengl-texture-state-usage-warning
    // https://www.khronos.org/opengl/wiki/Common_Mistakes
    // https://www.khronos.org/opengl/wiki/Texture
    // https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf (section 8 "Samplers and textures")

    // and make the Image or something like that class so user can define their own mipmaps
    // for example i give you couple images and specify the level for every image

    /**
     *
     *  1. Add parallax (height) mapping.
     *  2. Integrate the PBR program and Skybox program into engine.
     *  3. Add scene class.
     *  4. Add model class.
     *  5. Add more formats for textures.
     *  6. Upgrade mesh loader (adding sampler creation, valid texture loading, etc.).
     *  7. Fix animations.
     *  8. Add render targets.
     *  9. Add deffered shading.
     *  10. Add shadow mapping.
     *  11. Fix light classes
     *  12. Create renderer.
     *
     */

    render_target_sampler = tmanager->add(muse::SamplerSpecification{
        .s_mode = muse::SamplerAddressMode::CLAMP_TO_EDGE,
        .t_mode = muse::SamplerAddressMode::CLAMP_TO_EDGE,
        .r_mode = muse::SamplerAddressMode::CLAMP_TO_EDGE,
        .mag_filter = muse::SamplerFilter::NEAREST,
        .min_filter = muse::SamplerFilter::NEAREST,
        .use_mipmaps = false,
    });

    screen_albedo = tmanager->add_texture(width, height, render_target_sampler, muse::TextureFormat::RGBAF16);
    screen_normal = tmanager->add_texture(width, height, render_target_sampler, muse::TextureFormat::RGBAF16);
    screen_position = tmanager->add_texture(width, height, render_target_sampler, muse::TextureFormat::RGBAF16);
    screen_roughness = tmanager->add_texture(width, height, render_target_sampler, muse::TextureFormat::REDF16);
    screen_metallic = tmanager->add_texture(width, height, render_target_sampler, muse::TextureFormat::REDF16);
    screen_ao = tmanager->add_texture(width, height, render_target_sampler, muse::TextureFormat::REDF16);
    screen_depth = tmanager->add_texture(width, height, render_target_sampler, muse::TextureFormat::DEPTH_COMPONENT);

    render_target = std::make_unique<muse::GeneralRenderTarget>(screen_albedo,
                                                                screen_position,
                                                                screen_normal,
                                                                screen_depth,
                                                                screen_roughness,
                                                                screen_metallic,
                                                                screen_ao);

    screen_sys->set_value("color_target", screen_albedo->bindless_handle());
    screen_sys->set_value("normal_target", screen_normal->bindless_handle());
    screen_sys->set_value("position_target", screen_position->bindless_handle());
    screen_sys->set_value("roughness_target", screen_roughness->bindless_handle());
    screen_sys->set_value("metalness_target", screen_metallic->bindless_handle());
    screen_sys->set_value("ao_target", screen_ao->bindless_handle());

    zombie_diffuse = tmanager->load_texture("Assets/OtherModels/ZombieTexture.png", nullptr, true, false);

    skybox_sampler = tmanager->add(muse::SamplerSpecification{.s_mode = muse::SamplerAddressMode::CLAMP_TO_EDGE,
                                                              .t_mode = muse::SamplerAddressMode::CLAMP_TO_EDGE,
                                                              .r_mode = muse::SamplerAddressMode::CLAMP_TO_EDGE,
                                                              .use_mipmaps = false});

    hdr_texture = tmanager->load_texture("Assets/newport_loft.hdr", skybox_sampler, false, true);
    skybox_cubemap = tmanager->load_cubemap("Assets/skybox/left.jpg",
                                            "Assets/skybox/right.jpg",
                                            "Assets/skybox/top.jpg",
                                            "Assets/skybox/bottom.jpg",
                                            "Assets/skybox/front.jpg",
                                            "Assets/skybox/back.jpg",
                                            skybox_sampler,
                                            true,
                                            false);

    skybox_sys->set_value("skybox", skybox_cubemap->bindless_handle());

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