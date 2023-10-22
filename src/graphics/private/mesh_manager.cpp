#include "graphics/public/mesh_manager.h"

#include "graphics/public/animation.h"
#include "graphics/public/material.h"
#include "graphics/public/material_manager.h"
#include "graphics/public/mesh.h"
#include "graphics/public/skeleton.h"
#include "graphics/public/texture_manager.h"
#include "graphics/public/vertex.h"
#include "log/public/logger.h"

#if defined(AI_LMW_MAX_WEIGHTS)
#undef AI_LMW_MAX_WEIGHTS
#define AI_LMW_MAX_WEIGHTS 4
#endif

#define STB_IMAGE_IMPLEMENTATION 1
#include "graphics/public/stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cstdint>
#include <map>
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
glm::mat4 convert_matrix(const aiMatrix4x4 &m)
{
    return glm::mat4{m.a1, m.a2, m.a3, m.a4, m.b1, m.b2, m.b3, m.b4, m.c1, m.c2, m.c3, m.c4, m.d1, m.d2, m.d3, m.d4};
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
glm::vec3 convert_vec(const aiVector3D &vec)
{
    return glm::vec3{vec.x, vec.y, vec.z};
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
glm::vec4 convert_vec(const aiColor3D &vec)
{
    return glm::vec4{vec.r, vec.g, vec.b, 1.0f};
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
glm::vec4 convert_vec(const aiColor4D &vec)
{
    return glm::vec4{vec.r, vec.g, vec.b, vec.a};
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
glm::vec2 convert_vec(const aiVector2D &vec)
{
    return glm::vec2{vec.x, vec.y};
}

/**
 *
 *
 *  @param vec Assimp vector.
 *
 *  @return GLM vector.
 *
 */
glm::quat convert_quat(const aiQuaternion &q)
{
    return glm::quat{q.w, q.x, q.y, q.z};
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

    for (auto i = 0u; i < mesh->mNumVertices; i++)
    {
        const auto position = convert_vec(mesh->mVertices[i]);
        const auto normal = convert_vec(mesh->mNormals[i]);
        glm::vec4 color{0.0f};
        glm::vec2 texture_coordinates{0.0f};
        glm::vec3 tangent{0.0f};
        glm::vec3 bitangent{0.0f};

        if (mesh->HasVertexColors(0))
        {
            color = convert_vec(mesh->mColors[0][i]);
        }
        else
        {
            aiColor4D col{0.0f, 0.0f, 0.0f, 1.0f};
            mat->Get(AI_MATKEY_COLOR_DIFFUSE, col);

            color = convert_vec(col);
        }

        if (mesh->HasTextureCoords(0))
        {
            texture_coordinates = convert_vec(mesh->mTextureCoords[0][i]);
            tangent = convert_vec(mesh->mTangents[i]);
            bitangent = convert_vec(mesh->mBitangents[i]);
        }

        vertices.emplace_back(position, normal, color, texture_coordinates, tangent, bitangent);
    }

    // if there is no weights we return right away
    if (weights.empty())
    {
        return vertices;
    }

    for (auto i = 0u; i < vertices.size(); i++)
    {
        auto &vertex = vertices[weights[i].vertex_id];

        for (auto j = 0u; j < 4; j++)
        {
            vertex.bone_ids[j] = weights[i].index;
            vertex.weights[j] = weights[i].weight;
        }
    }

    return vertices;
}

/**
 *
 *  Process all children of the given bone.
 *
 *  @param bone Bone to process all of it's children.
 *  @param cached_bones Assimp bones that cached.
 *
 */
std::vector<muse::Bone> process_children(const aiBone *bone, std::vector<aiBone *> &cached_bones)
{
    auto *node = bone->mNode;

    if (!node->mNumChildren)
    {
        return {};
    }

    std::vector<muse::Bone> children{};

    for (auto i = 0u; i < node->mNumChildren; i++)
    {
        auto *child = node->mChildren[i];

        muse::Bone bone{child->mName.data, {}, convert_matrix(child->mTransformation), -1, {}};

        auto it = std::find_if(std::begin(cached_bones), std::end(cached_bones), [&](const auto &bone)
                               {
                                   return bone->mNode == child;
                               });

        if (it != std::end(cached_bones))
        {
            bone = {child->mName.data,
                    convert_matrix((*it)->mOffsetMatrix),
                    convert_matrix(child->mTransformation),
                    -1,
                    process_children(*it, cached_bones)};
        }

        children.push_back(bone);
    }

    return children;
}

/**
 *
 *  Get root bone.
 *
 *  @param scene Scene with all bones.
 *  @param cached_bones Cached assimp bones.
 *
 */
const aiBone *find_root_bone(const aiScene *scene, const std::vector<aiBone *> &cached_bones)
{
    std::vector<aiString> bone_names{};

    // Copy all bone names.
    for (auto i = 0u; i < scene->mNumMeshes; i++)
    {
        auto *mesh = scene->mMeshes[i];

        for (auto j = 0u; j < mesh->mNumBones; j++)
        {
            bone_names.push_back(mesh->mBones[j]->mName);
        }
    }

    // Loop all bones and compare their name with parent's name of node
    // and if it matches, increment counter
    // and if counter is 0 means no names match the parent's
    // name so it's the root bone
    for (const auto &bone : cached_bones)
    {
        auto *node = bone->mNode;
        auto counter = 0u;

        for (auto k = 0u; k < bone_names.size(); k++)
        {
            if (node->mParent->mName == bone_names[k])
            {
                counter++;
            }
        }

        if (!counter)
        {
            LOG_INFO(NameOfRoot, "Name: {}", bone->mName.data);
            return bone;
        }
    }

    return nullptr;
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

    LOG_INFO(TotalVertices, "Total vertex count: {}", mesh->mNumVertices);

    return weights;
}

/**
 *
 *  Setup all bone indices.
 *
 *  @param bone_name_to_index Map to setup.
 *  @param index Internal counter we will increment every time when found new bone.
 *  @param bone This argument should be root bone of skeleton all of the times.
 *
 */
void setup_bone_indices(BoneNameToIndexMap &bone_name_to_index, std::uint32_t &index, const muse::Bone &bone)
{
    // Process provided bone
    bone_name_to_index[bone.name] = index;

    // Process all of bone's children and for every bone
    // increment counter and process that bone
    for (const auto &children_bone : bone.children)
    {
        ++index;
        setup_bone_indices(bone_name_to_index, index, children_bone);
    }
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
    // We cache the assimp bones to not have loop over all meshes in scene again and again
    // but just do it one time
    std::vector<aiBone *> cache{};
    for (auto i = 0u; i < scene->mNumMeshes; i++)
    {
        auto *mesh = scene->mMeshes[i];

        for (auto j = 0u; j < mesh->mNumBones; j++)
        {
            auto *bone = mesh->mBones[j];

            cache.push_back(bone);
        }
    }

    if (!cache.size())
    {
        LOG_WARN(NoSkeleton, "Model doesn't have any bones, returning default skeleton");
        return std::make_tuple(muse::Skeleton{}, BoneNameToIndexMap{});
    }

    // Get root bone.
    auto root = find_root_bone(scene, cache);

    cache.erase(std::find_if(std::cbegin(cache), std::cend(cache), [&](const aiBone *bone)
                             {
                                 return bone == root;
                             }));

    auto children = process_children(root, cache);

    muse::Bone root_bone{
        root->mName.data,
        convert_matrix(root->mOffsetMatrix),
        convert_matrix(root->mNode->mTransformation),
        -1, children};

    // Needed to setup indices
    std::uint32_t bone_counter = 0u;
    BoneNameToIndexMap bone_name_to_index{};

    // Setup all of indices.
    setup_bone_indices(bone_name_to_index, bone_counter, root_bone);

    for (const auto &[name, index] : bone_name_to_index)
    {
        LOG_INFO(Map, "Name: {}\nIndex: {}", name, index);
    }

    return std::make_tuple(muse::Skeleton{root_bone}, std::move(bone_name_to_index));
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

std::uint32_t process_texture(const aiMaterial *mat,
                              const aiScene *scene,
                              aiTextureType type,
                              muse::TextureManager *tmanager,
                              const std::string &dir)
{
    aiString path{};

    // Diffuse is only texture that we can gamma correct
    auto format = type == aiTextureType_DIFFUSE ? muse::TextureFormat::SRGB_ALPHA : muse::TextureFormat::RGBA;

    if (mat->GetTextureCount(type) != 0 && mat->GetTexture(type, 0, &path) == aiReturn_SUCCESS)
    {
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
                // FIXME: Determine if we should flip image or not
                stbi_set_flip_vertically_on_load(true);

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
            return tmanager->add(data, w, h, -1, color_channels, format)->index();
        }
        else
        {
            // we want all textures/images registered in texture manager
            return tmanager->load(dir + std::string(path.data), -1, format, false)->index();
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

        auto duration = static_cast<float>(animation->mDuration);

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

                muse::Keyframe keyframe{static_cast<float>(channel->mPositionKeys[j].mTime),
                                        muse::Transform{position, rotation, scale}};

                keyframes.push_back(keyframe);
            }

            // If doesn't exist in map create new element
            frames[name] = keyframes;
        }

        animations.emplace_back(animation->mName.data, duration, ticks_per_second, std::move(frames));
    }

    return animations;
}

std::vector<muse::Mesh *> process_meshes(muse::MeshManager *manager,
                                         muse::TextureManager *tmanager,
                                         muse::MaterialManager *mmanager,
                                         const muse::AnimationCallback &animation_callback,
                                         const aiScene *scene,
                                         const std::string &dir)
{
    std::vector<muse::Mesh *> meshes{};

    // one skeleton per model
    auto [skeleton, bone_name_to_index_map] = process_skeleton(scene);

    for (auto i = 0u; i < scene->mNumMeshes; i++)
    {
        auto *mesh = scene->mMeshes[i];
        auto *mat = scene->mMaterials[mesh->mMaterialIndex];

        auto weights = process_weights(mesh, bone_name_to_index_map);
        auto vertices = process_vertices(mesh, weights, mat);
        auto indices = process_indices(mesh);
        auto mat_index = process_material(mat, scene, tmanager, mmanager, dir);

        meshes.push_back(manager->create(vertices, indices, mat_index));
    }

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
                                      bool flip_uvs)
{
    auto flags = flip_uvs ? aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_PopulateArmatureData |
                                aiProcess_LimitBoneWeights | aiProcess_FlipUVs
                          : aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_PopulateArmatureData |
                                aiProcess_LimitBoneWeights;

    Assimp::Importer importer{};
    importer.SetPropertyInteger(AI_CONFIG_IMPORT_REMOVE_EMPTY_BONES, 0);
    importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);

    const auto *scene = importer.ReadFile(filename, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_ERROR(MeshManager, "failed to load file\nError string: {}", importer.GetErrorString());
    }

    auto pos = filename.find_last_of('/');
    std::string dir = filename.substr(0, pos + 1);

    return process_meshes(this, tmanager_, mmanager_, animation_callback, scene, dir);
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