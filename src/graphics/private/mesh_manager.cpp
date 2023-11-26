#include "graphics/public/mesh_manager.h"

#include "graphics/public/animation/animation.h"
#include "graphics/public/animation/skeleton.h"
#include "graphics/public/material.h"
#include "graphics/public/material_manager.h"
#include "graphics/public/mesh.h"
#include "graphics/public/texture_manager.h"
#include "graphics/public/vertex.h"
#include "log/public/logger.h"
#include "utils/public/vector2.h"

#define STB_IMAGE_IMPLEMENTATION 1
#include "graphics/public/stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cstdint>
#include <map>
#include <queue>
#include <stack>

/** Aliases */
using BoneToNodeMap = std::unordered_map<const aiBone *, aiNode *>;
using BoneNameToIndexMap = std::map<std::string, std::uint32_t, std::less<>>;

/**
 *
 *  Convert assimp matrix to glm matrix.
 *
 *  @param m Assimp matrix.
 *
 *  @return GLM matrix.
 *
 */
muse::Matrix4 convert_matrix(const aiMatrix4x4 &m)
{
    return muse::Matrix4{m.a1, m.a2, m.a3, m.a4,
                         m.b1, m.b2, m.b3, m.b4,
                         m.c1, m.c2, m.c3, m.c4,
                         m.d1, m.d2, m.d3, m.d4};
}

/**
 *
 *  Convert assimp vector 3d to GLM vector.
 *
 *  @param vec Assimp vector.
 *
 *  @return GLM vector.
 *
 */
muse::Vector3 convert_vec(const aiVector3D &vec)
{
    return muse::Vector3{vec.x, vec.y, vec.z};
}

/**
 *
 *  Convert assimp vector 3d to GLM vector.
 *
 *  @param vec Assimp vector.
 *
 *  @return GLM vector.
 *
 */
muse::Vector4 convert_vec(const aiColor3D &vec)
{
    return muse::Vector4{vec.r, vec.g, vec.b, 1.0f};
}

/**
 *
 *  Convert assimp RGBA color to GLM vector.
 *
 *  @param vec Assimp vector.
 *
 *  @return GLM vector.
 *
 */
muse::Vector4 convert_vec(const aiColor4D &vec)
{
    return muse::Vector4{vec.r, vec.g, vec.b, vec.a};
}

/**
 *
 *  Convert assimp vector 2d to GLM vector.
 *
 *  @param vec Assimp vector.
 *
 *  @return GLM vector.
 *
 */
muse::Vector2 convert_vec(const aiVector2D &vec)
{
    return muse::Vector2{vec.x, vec.y};
}

/**
 *
 *  Convert assimp quaternion to Engine quaternion
 *
 *  @param q Assimp quaternion.
 *
 *  @return Engine vector.
 *
 */
muse::Quaternion convert_quat(const aiQuaternion &q)
{
    return muse::Quaternion{q.x, q.y, q.z, q.w};
}

/**
 *
 *  Process all vertices in mesh.
 *
 *  @param mesh Mesh to process.
 *  @param weights Weights to setup.
 *  @param mat Mesh's material.
 *
 *  @return Vector of vertices in order that they were stored in mesh.
 *
 */
std::vector<muse::Vertex> process_vertices(const aiMesh *mesh, std::vector<muse::Weight> &weights, aiMaterial *mat)
{
    std::vector<muse::Vertex> vertices{};

    auto has_normals = mesh->HasNormals();
    auto has_colors = mesh->HasVertexColors(0);
    auto has_tex_coords = mesh->HasTextureCoords(0);
    auto has_tangents = mesh->HasTangentsAndBitangents();

    for (auto i = 0u; i < mesh->mNumVertices; i++)
    {
        const auto position = convert_vec(mesh->mVertices[i]);
        muse::Vector3 tangent{1.0f, 0.0f, 0.0f};
        muse::Vector3 normal{0.0f, 1.0f, 0.0f};
        muse::Vector3 bitangent{0.0f, 0.0f, 1.0f};
        muse::Vector4 color{0.0f};
        muse::Vector2 texture_coordinates{0.0f};

        if (has_normals)
        {
            normal = convert_vec(mesh->mNormals[i]);
        }

        if (has_colors)
        {
            color = convert_vec(mesh->mColors[0][i]);
        }
        else
        {
            aiColor4D col{0.0f, 0.0f, 0.0f, 1.0f};
            mat->Get(AI_MATKEY_COLOR_DIFFUSE, col);

            color = convert_vec(col);
        }

        if (has_tex_coords)
        {
            auto tex = mesh->mTextureCoords[0][i];
            texture_coordinates = muse::Vector2(tex.x, tex.y);

            if (has_tangents)
            {
                tangent = convert_vec(mesh->mTangents[i]);
                bitangent = convert_vec(mesh->mBitangents[i]);
            }
        }

        vertices.emplace_back(position, normal, color, texture_coordinates, tangent, bitangent);
    }

    // if there is no weights we return right away
    if (weights.empty())
    {
        return vertices;
    }

    std::vector<std::uint32_t> bone_indices(vertices.size());
    for (const auto &[bone, id, weight] : weights)
    {
        if (weight == 0.0f)
        {
            continue;
        }

        if (bone_indices[id] >= 4)
        {
            LOG_WARN(BoneWeights, "Too many weights {} {}", id, weight);
            continue;
        }

        vertices[id].bone_ids[bone_indices[id]] = bone;
        vertices[id].weights[bone_indices[id]] = weight;

        bone_indices[id]++;
    }

    return vertices;
}

/**
 *
 *  Setup weights of vertices.
 *
 *  @param mesh Mesh to process weights of.
 *  @param map Map that's has it's key as bone name and it's value as it's index.
 *
 */
std::vector<muse::Weight> process_weights(const aiMesh *mesh, BoneNameToIndexMap &map)
{
    // means model doesn't have any bones so returning empty vector
    if (map.empty())
    {
        return {};
    }

    std::vector<muse::Weight> weights{};

    for (auto i = 0u; i < mesh->mNumBones; i++)
    {
        auto *bone = mesh->mBones[i];

        for (auto k = 0u; k < bone->mNumWeights; k++)
        {
            auto weight = bone->mWeights[k];

            if (weight.mWeight != 0.0f)
            {
                weights.emplace_back(map[bone->mName.data], weight.mVertexId, weight.mWeight);
            }
        }
    }

    return weights;
}

/**
 *
 *  Setup all bone indices.
 *
 *  @param bone_name_to_index Map to setup.
 *  @param index External counter we will increment every time when found new bone.
 *  @param bones Ordered bones.
 *
 */
void setup_bone_indices(BoneNameToIndexMap &bone_name_to_index, std::int32_t &index, std::vector<muse::Bone> &bones)
{
    for (auto &bone : bones)
    {
        index++;
        bone_name_to_index[bone.name] = index;
        bone.index = index;
    }
}

aiBone *find_bone(const std::string &name, const aiScene *scene)
{
    for (auto i = 0u; i < scene->mNumMeshes; i++)
    {
        auto *mesh = scene->mMeshes[i];

        for (auto k = 0u; k < mesh->mNumBones; k++)
        {
            auto *assimp_bone = mesh->mBones[k];

            if (std::string(assimp_bone->mName.data) == name)
            {
                return assimp_bone;
            }
        }
    }

    return nullptr;
}

/**
 *
 *  Process skeleton of mesh.
 *
 *  @param vertices Vertices to setup weights of.
 *  @param mesh Mesh to process skeleton of.
 *  @param scene Assimp scene.
 *
 *  @return Tuple [skeleton, bone_name_to_index_map].
 *
 */
std::tuple<muse::Skeleton, BoneNameToIndexMap> process_skeleton(const aiScene *scene)
{
    // Read the node hierarchy and extract all bones from it
    std::vector<muse::Bone> cache;
    std::stack<aiNode *> stack{};
    stack.emplace(scene->mRootNode);

    do
    {
        const auto node = stack.top();
        stack.pop();

        std::string name = node->mName.data;

        muse::Bone bone{node->mName.data,
                        muse::Matrix4{},
                        convert_matrix(node->mTransformation),
                        -1,
                        node->mParent ? node->mParent->mName.data : ""};

        if (auto *ai_bone = find_bone(bone.name, scene); ai_bone)
        {
            bone = {node->mName.data,
                    convert_matrix(ai_bone->mOffsetMatrix),
                    convert_matrix(node->mTransformation),
                    -1,
                    node->mParent ? node->mParent->mName.data : ""};
        }

        cache.emplace_back(bone);

        for (auto i = 0u; i < node->mNumChildren; i++)
        {
            stack.emplace(node->mChildren[i]);
        }

    } while (!stack.empty());

    if (!cache.size())
    {
        LOG_WARN(NoSkeleton, "Model doesn't have any bones, returning default skeleton");
        return std::make_tuple(muse::Skeleton{}, BoneNameToIndexMap{});
    }

    auto root = std::begin(cache);

    std::vector<std::size_t> parents{};
    std::vector<muse::Bone> sorted_bones{};

    std::queue<std::vector<muse::Bone>::iterator> queue;
    queue.emplace(root);
    parents.emplace_back(std::numeric_limits<std::size_t>::max()); // for root bone there's no parent so just some weird value

    // algorithm is pretty simple just start with a queue add root bone to it
    // move it to back of out new list then get it's name find all bone
    // whose this bone is parent and emplace them queue to process later and emplace
    // index of this bone in new list to parents list and repeat it until there's no bones to process.
    do
    {
        const auto iter = queue.front();
        queue.pop();

        // move current bone to the back of out bone vector
        std::move(iter, iter + 1, std::back_inserter(sorted_bones));

        const auto name = sorted_bones.back().name;

        // for this bone find it's children and emplace them after the parent into bone vector
        for (auto i = std::begin(cache); i != std::end(cache); i++)
        {
            if (i->parent == name)
            {
                queue.emplace(i);
                parents.emplace_back(sorted_bones.size() - 1); // this will give us the index of parent
            }
        }

    } while (!queue.empty());

    // Need to setup indices
    std::int32_t bone_counter = -1;
    BoneNameToIndexMap bone_name_to_index{};

    // Setup all of indices.
    setup_bone_indices(bone_name_to_index, bone_counter, sorted_bones);

    return std::make_tuple(muse::Skeleton{sorted_bones, parents}, std::move(bone_name_to_index));
}

std::vector<std::uint32_t> process_indices(const aiMesh *mesh)
{
    std::vector<std::uint32_t> indices{};

    for (auto i = 0u; i < mesh->mNumFaces; i++)
    {
        auto face = mesh->mFaces[i];

        for (auto k = 0u; k < face.mNumIndices; k++)
        {
            indices.push_back(face.mIndices[k]);
        }
    }

    return indices;
}

muse::SamplerAddressMode convert_mode(aiTextureMapMode mode)
{
    auto engine_mode = muse::SamplerAddressMode::REPEAT;

    switch (mode)
    {
        case aiTextureMapMode_Clamp: engine_mode = muse::SamplerAddressMode::CLAMP_TO_EDGE; break;
        case aiTextureMapMode_Decal: engine_mode = muse::SamplerAddressMode::CLAMP_TO_BORDER; break;
        case aiTextureMapMode_Mirror: engine_mode = muse::SamplerAddressMode::MIRRORED_REPEAT; break;
        default: break;
    }

    return engine_mode;
}

muse::Sampler *create_sampler(muse::TextureManager *tmanager,
                              const aiMaterial *mat,
                              aiTextureType type)
{
    // FIXME: Try to get more of attributes for sampler
    auto *default_sampler = tmanager->default_texture_sampler();

    muse::SamplerSpecification spec{};

    aiTextureMapMode u_mode;
    aiTextureMapMode v_mode;

    if (mat->Get(AI_MATKEY_MAPPINGMODE_U(type, 0), u_mode) == aiReturn_SUCCESS)
    {
        spec.s_mode = convert_mode(u_mode);
    }

    if (mat->Get(AI_MATKEY_MAPPINGMODE_V(type, 0), v_mode) == aiReturn_SUCCESS)
    {
        spec.t_mode = convert_mode(v_mode);
    }

    // if this specification matches with default sampler's specification there's no need to create another same sampler
    if (default_sampler->specification() == spec)
    {
        return nullptr;
    }

    return tmanager->add(spec);
}

std::uint32_t process_texture(const aiMaterial *mat,
                              const aiScene *scene,
                              aiTextureType type,
                              muse::TextureManager *tmanager,
                              const std::string &dir)
{
    aiString path{};

    // Diffuse is only texture that we can gamma correct
    auto format = type == aiTextureType_DIFFUSE ? muse::TextureFormat::SRGB_ALPHA : muse::TextureFormat::RGBA;

    if (mat->GetTextureCount(type) != 0 && mat->GetTexture(type, 0, &path, nullptr, nullptr, nullptr, nullptr, nullptr) == aiReturn_SUCCESS)
    {
        auto sampler = create_sampler(tmanager, mat, type);

        // if it is embedded texture means it is stored in model file
        // we use assimp to help us load it
        if (auto tex = scene->GetEmbeddedTexture(path.data); tex)
        {
            auto w = 0;
            auto h = 0;
            auto color_channels = 0;
            std::byte *data = nullptr;

            // if height is 0 we need to decode image data, we will use stb_image for this
            if (!tex->mHeight)
            {
                data = reinterpret_cast<std::byte *>(stbi_load_from_memory(reinterpret_cast<stbi_uc *>(tex->pcData),
                                                                           tex->mWidth,
                                                                           &w,
                                                                           &h,
                                                                           &color_channels,
                                                                           4));
            }
            else // else we just proceed
            {
                w = tex->mWidth;
                h = tex->mHeight;
                color_channels = tex->CheckFormat("jpg") ? 3 : 4;
                data = reinterpret_cast<std::byte *>(tex->pcData);
            }

            // we want all textures/images registered in texture manager
            return tmanager->add(data, w, h, sampler, color_channels, format)->index();
        }
        else
        {
            // we want all textures/images registered in texture manager
            return tmanager->load(dir + std::string(path.data), sampler, format, false)->index();
        }
    }

    return std::numeric_limits<std::uint32_t>::max();
}

std::uint32_t process_material(const aiMaterial *mat,
                               const aiScene *scene,
                               muse::TextureManager *tmanager,
                               muse::MaterialManager *mmanager,
                               const std::string &dir)
{
    // Process all textures
    muse::MaterialIndices indices{};
    indices.albedo = process_texture(mat, scene, aiTextureType_DIFFUSE, tmanager, dir);

    mmanager->add(indices, mat->GetName().data);

    return mmanager->counter();
}

std::vector<muse::Animation> process_animations(const aiScene *scene)
{
    std::vector<muse::Animation> animations{};

    for (auto i = 0u; i < scene->mNumAnimations; i++)
    {
        auto animation = scene->mAnimations[i];

        auto ticks_per_second = static_cast<float>(animation->mTicksPerSecond);
        if (ticks_per_second == 0.0f)
        {
            ticks_per_second = 25.0f;
        }

        const std::chrono::milliseconds tick_time{static_cast<std::uint32_t>((1.0f / ticks_per_second) * 1000.0f)};
        const auto duration = static_cast<std::uint32_t>(animation->mDuration) * tick_time;

        std::unordered_map<std::string, std::vector<muse::Keyframe>> frames{};

        for (auto k = 0u; k < animation->mNumChannels; k++)
        {
            auto channel = animation->mChannels[k];

            std::vector<muse::Keyframe> keyframes{};
            std::string name = channel->mNodeName.data;

            for (auto j = 0u; j < channel->mNumPositionKeys; j++)
            {
                const auto position = convert_vec(channel->mPositionKeys[j].mValue);
                const auto rotation = convert_quat(channel->mRotationKeys[j].mValue);
                const auto scale = convert_vec(channel->mScalingKeys[j].mValue);
                const auto time = static_cast<std::uint32_t>(channel->mPositionKeys[j].mTime);

                muse::Keyframe keyframe{time * tick_time,
                                        muse::Transform{position, rotation, scale}};

                keyframes.push_back(keyframe);
            }

            // If doesn't exist in map create new element
            frames[name] = keyframes;
        }

        animations.emplace_back(animation->mName.data, duration, std::move(frames));
    }

    return animations;
}

std::vector<muse::Mesh *> process_meshes(muse::MeshManager *manager,
                                         muse::TextureManager *tmanager,
                                         muse::MaterialManager *mmanager,
                                         const muse::AnimationCallback &animation_callback,
                                         const aiScene *scene,
                                         const std::string &dir,
                                         bool load_materials)
{
    std::vector<muse::Mesh *> meshes{};

    // one skeleton per model
    auto [skeleton, bone_name_to_index_map] = process_skeleton(scene);

    std::stack<aiNode *> nodes{};
    nodes.emplace(scene->mRootNode);

    do
    {
        auto *node = nodes.top();
        nodes.pop();

        for (auto i = 0u; i < node->mNumMeshes; i++)
        {
            auto *mesh = scene->mMeshes[node->mMeshes[i]];
            auto *mat = scene->mMaterials[mesh->mMaterialIndex];

            auto weights = process_weights(mesh, bone_name_to_index_map);
            auto vertices = process_vertices(mesh, weights, mat);
            auto indices = process_indices(mesh);

            std::uint32_t mat_index = std::numeric_limits<std::uint32_t>::max();
            if (load_materials)
            {
                mat_index = process_material(mat, scene, tmanager, mmanager, dir);
            }

            auto *loaded_mesh = manager->create(vertices, indices, mat_index);

            auto node_transform = convert_matrix(node->mTransformation);
            muse::Transform mesh_transform(node_transform);
            loaded_mesh->set_transform(mesh_transform);

            meshes.push_back(loaded_mesh);
        }

        for (auto i = 0u; i < node->mNumChildren; i++)
        {
            nodes.emplace(node->mChildren[i]);
        }
    } while (!nodes.empty());

    if (animation_callback != nullptr && scene->mNumAnimations != 0)
    {
        animation_callback(process_animations(scene), std::move(skeleton), std::move(bone_name_to_index_map));
    }

    return meshes;
}

namespace muse
{

MeshManager::MeshManager(TextureManager *tmanager, MaterialManager *mmanager)
    : meshes_()
    , tmanager_(tmanager)
    , mmanager_(mmanager)
{
}

std::vector<Mesh *> MeshManager::load(const std::string &filename,
                                      const AnimationCallback &animation_callback,
                                      bool load_materials,
                                      bool flip_uvs)
{
    auto flags = flip_uvs ? aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_PopulateArmatureData | aiProcess_LimitBoneWeights | aiProcess_FlipUVs
                          : aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_PopulateArmatureData | aiProcess_LimitBoneWeights;

    Assimp::Importer importer{};

    const auto *scene = importer.ReadFile(filename, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_ERROR(MeshManager, "failed to load file\nError string: {}", importer.GetErrorString());
    }

    auto pos = filename.find_last_of('/');
    std::string dir = filename.substr(0, pos + 1);

    return process_meshes(this, tmanager_, mmanager_, animation_callback, scene, dir, load_materials);
}

Mesh *MeshManager::mesh(std::uint32_t index)
{
    return meshes_.at(index).get();
}

void MeshManager::remove(std::uint32_t index)
{
    meshes_.erase(std::begin(meshes_) + index);
}

Mesh *MeshManager::create(const std::vector<Vertex> &vertices,
                          const std::vector<std::uint32_t> &indices,
                          std::uint32_t material_index)
{
    return meshes_.emplace_back(std::make_unique<Mesh>(vertices, indices, material_index)).get();
}
}