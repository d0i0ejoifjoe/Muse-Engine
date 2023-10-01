#include "graphics/public/mesh_manager.h"
#include "graphics/public/mesh.h"
#include "graphics/public/vertex.h"
#include "graphics/public/skeleton.h"
#include "graphics/public/animation.h"
#include "log/public/logger.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cstdint>
#include <unordered_map>

/** Aliases */
using BoneToNodeMap = std::unordered_map<const aiBone*, aiNode*>;
using BoneNameToIndexMap = std::unordered_map<std::string, std::uint32_t>;

glm::mat4 convert_matrix(const aiMatrix4x4& m)
{
    return glm::mat4{m.a1, m.a2, m.a3, m.a4,
                     m.b1, m.b2, m.b3, m.b4,
                     m.c1, m.c2, m.c3, m.c4,
                     m.d1, m.d2, m.d3, m.d4};
}

glm::vec3 convert_vec(const aiVector3D& vec)
{
    return glm::vec3{vec.x, vec.y, vec.z};
}

glm::vec4 convert_vec(const aiColor4D& vec)
{
    return glm::vec4{vec.r, vec.g, vec.b, vec.a};
}

glm::vec2 convert_vec(const aiVector2D& vec)
{
    return glm::vec2{vec.x, vec.y};
}

glm::quat convert_quat(const aiQuaternion& q)
{
    return glm::quat{q.w, q.x, q.y, q.z};
}

/**
 * 
 *  Process all vertices in mesh.
 * 
 *  @param mesh Mesh to process.
 * 
 *  @return Vector of vertices in order that they were stored in mesh.
 * 
*/
std::vector<muse::Vertex> process_vertices(const aiMesh* mesh)
{
    std::vector<muse::Vertex> vertices{};

    for(auto i = 0u; i < mesh->mNumVertices; i++)
    {
        const auto position = convert_vec(mesh->mVertices[i]);
        const auto normal = convert_vec(mesh->mNormals[i]);
        glm::vec4 color{};
        glm::vec2 texture_coordinates{};

        if(mesh->HasVertexColors(0))
        {
            color = convert_vec(mesh->mColors[0][i]);
        }

        if(mesh->HasTextureCoords(0))
        {
            texture_coordinates = convert_vec(mesh->mTextureCoords[0][i]);
        }

        const auto tangent = convert_vec(mesh->mTangents[i]);
        const auto bitangent = convert_vec(mesh->mBitangents[i]); 
    
        vertices.emplace_back(position, normal, color, texture_coordinates, tangent, bitangent);
    }

    return vertices;
}

/**
 * 
 *  Find bone by name in mesh.
 * 
 *  @param name Name of bone.
 *  @param mesh Mesh to find bone in.
 * 
*/
const aiBone* find_bone(const aiString& name, const aiMesh* mesh)
{
    // Loop all bones and check if name matches the bone name
    for(auto i = 0u; i < mesh->mNumBones; i++)
    {
        auto bone = mesh->mBones[i];
        if(bone->mName == name)
        {
            return bone;
        }
    }

    return nullptr;
}

/**
 * 
 *  Process all children of the given bone.
 * 
 *  @param bone Bone to process all of it's children.
 *  @param mesh Mesh to find the children in.
 *  @param map Map that maps bone to node.
 * 
*/
std::vector<muse::Bone> process_children(const aiBone* bone, const aiMesh* mesh, BoneToNodeMap& map)
{
    auto node = map[bone];

    if(node->mNumChildren == 0)
    {
        return {};
    }

    std::vector<muse::Bone> children{};
    for(auto i = 0u; i < node->mNumChildren; i++)
    {
        auto children_node = node->mChildren[i];
        auto children_bone = find_bone(children_node->mName, mesh);
        
        children.emplace_back(children_bone->mName.data,
                              convert_matrix(children_bone->mOffsetMatrix),
                              convert_matrix(children_node->mTransformation),
                              -1,
                              process_children(children_bone, mesh, map));
    }

    return children;
} 

/**
 * 
 *  Get root bone.
 * 
 *  @param mesh Mesh with all bones.
 *  @param map Map that maps bone to node.
 * 
*/
const aiBone* find_root_bone(const aiMesh* mesh, BoneToNodeMap& map)
{
    std::vector<aiString> bone_names{};

    // Copy all bone names.
    for(auto i = 0u; i < mesh->mNumBones; i++)
    {
        bone_names.push_back(mesh->mBones[i]->mName);
    }

    // Loop all bones and compare name of parent of node
    // and if it's matches increment counter
    // and if counter is 0 means no names match the parent's
    // name so it's the root bone
    for(auto i = 0u; i < mesh->mNumBones; i++)
    {
        auto node = map[mesh->mBones[i]];

        auto counter = 0u;

        for(auto k = 0u; k < bone_names.size(); k++)
        {
            if(node->mParent->mName == bone_names[k])
            {
                counter++;
            }
        }

        if(counter == 0)
        {
            return mesh->mBones[i];
        }
    }

    return nullptr;
}

/**
 * 
 *  Setup weights of vertices.
 * 
 *  @param vertices Vertices to setup weights of.
 *  @param mesh Mesh to find the weights in.
 *  @param bone_name_to_index Map that's has it's key as bone name and it's value as it's index.
 * 
*/
void process_weights(std::vector<muse::Vertex>& vertices,
                     const aiMesh* mesh,
                     BoneNameToIndexMap& bone_name_to_index)
{
    for(auto i = 0u; i < mesh->mNumBones; i++)
    {
        auto bone = mesh->mBones[i];

        for(auto k = 0u; k < bone->mNumWeights; k++)
        {
            auto weight = bone->mWeights[k];
            auto& vertex = vertices.at(weight.mVertexId);
            vertex.weights[k] = muse::Weight{bone_name_to_index[bone->mName.data], weight.mWeight};  
        }
    }
}

/**
 * 
 *  Find node by name.
 * 
 *  @param name Name of node to find.
 *  @param node Node to compare it's name with provided name (from start always supply scene->mRootNode
 *                                                            if you want to look for node in whole hierarchy).
 *  @param out_node If node found set the this pointer to the found node.
 * 
*/
void find_node(aiString& name,
               aiNode* node,
               aiNode** out_node)
{
    // If node's name matches provided name set the pointer to the node.
    if(name == node->mName)
    {
        *out_node = node;
    }

    // Else we loop through all of it's children to look for node there if 
    // has no children loop won't start anyway, so no need to add any unnecessary
    // if statements
    for(auto i = 0u; i < node->mNumChildren; i++)
    {
        find_node(name, node->mChildren[i], out_node);   
    }
}

/**
 * 
 *  Create bone to node map.
 * 
 *  @param mesh Mesh that contains all bones for map.
 *  @param scene Scene that contains all nodes for map.
 * 
 *  @return New map.
 * 
*/
BoneToNodeMap create_bone_to_node_map(const aiMesh* mesh,
                                      const aiScene* scene)
{
    BoneToNodeMap map{};

    for(auto i = 0u; i < mesh->mNumBones; i++)
    {
        auto bone = mesh->mBones[i];
        aiNode* out_node = nullptr;

        find_node(bone->mName, scene->mRootNode, std::addressof(out_node));
        map[bone] = out_node;
    }

    return map;
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
void setup_bone_indices(BoneNameToIndexMap& bone_name_to_index,
                        std::uint32_t& index,
                        const muse::Bone& bone)
{
    // Process provided bone
    bone_name_to_index[bone.name] = index;

    // Process all of bone's children and for every bone
    // increment counter and process that bone
    for(const auto& children_bone : bone.children)
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
*/
muse::Skeleton process_skeleton(std::vector<muse::Vertex>& vertices, const aiMesh* mesh, const aiScene* scene)
{
    std::vector<muse::Bone> bones{};

    std::uint32_t bone_counter = 0u;
    BoneNameToIndexMap bone_name_to_index{};

    // Get bone to node map.
    BoneToNodeMap bone_to_node = create_bone_to_node_map(mesh, scene);

    // Get root bone.
    auto root = find_root_bone(mesh, bone_to_node);
    auto children = process_children(root, mesh, bone_to_node);

    muse::Bone root_bone{root->mName.data,
                         convert_matrix(root->mOffsetMatrix),
                         convert_matrix(bone_to_node[root]->mTransformation),
                         -1,
                         children};

    // Setup all of indices.
    setup_bone_indices(bone_name_to_index, bone_counter, root_bone);
    
    // Setup all of weights.
    process_weights(vertices, mesh, bone_name_to_index);

    return muse::Skeleton{root_bone};
}

std::vector<std::uint32_t> process_indices(const aiMesh* mesh)
{
    std::vector<std::uint32_t> indices{};

    for(auto i = 0u; i < mesh->mNumFaces; i++)
    {
        auto face = mesh->mFaces[i];

        for(auto k = 0u; k < face.mNumIndices; k++)
        {
            indices.push_back(face.mIndices[k]);
        }
    }

    return indices;
}

std::unique_ptr<muse::Mesh> process_mesh(const aiScene* scene)
{
    auto mesh = scene->mMeshes[0];

    auto vertices = process_vertices(mesh);
    auto skeleton = process_skeleton(vertices, mesh, scene);
    auto indices = process_indices(mesh);

    return std::make_unique<muse::Mesh>(vertices, indices, skeleton);
}

std::vector<muse::Animation> process_animations(const aiScene* scene)
{
    std::vector<muse::Animation> animations{};

    for(auto i = 0u; i < scene->mNumAnimations; i++)
    {
        auto animation = scene->mAnimations[i];

        auto ticks_per_second = static_cast<float>(animation->mTicksPerSecond);
        if(ticks_per_second == 0.0f)
        {
            ticks_per_second = 25.0f;
        }

        auto duration = static_cast<float>(animation->mDuration);

        std::unordered_map<std::string, std::vector<muse::Keyframe>> frames{};

        for(auto k = 0u; k < animation->mNumChannels; k++)
        {
            auto channel = animation->mChannels[k];

            std::vector<muse::Keyframe> keyframes{};
            std::string name = channel->mNodeName.data;

            for(auto j = 0u; j < channel->mNumPositionKeys; j++)
            {
                const auto position = convert_vec(channel->mPositionKeys[j].mValue);
                const auto rotation = convert_quat(channel->mRotationKeys[j].mValue);
                const auto scale = convert_vec(channel->mScalingKeys[j].mValue);

                muse::Keyframe keyframe{
                    static_cast<float>(channel->mPositionKeys[j].mTime),
                    muse::Transform{position,
                                    rotation,
                                    scale}
                };

                keyframes.push_back(keyframe);
            }

            // If doesn't exist in map create new element
            frames[name] = keyframes;
        }

        animations.emplace_back(animation->mName.data, duration, ticks_per_second, std::move(frames));
    }

    return animations;
}

namespace muse
{
    Mesh* MeshManager::load(const std::string& filename,
                            bool flip_uvs)
    {
        auto flags = flip_uvs ? aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights | aiProcess_FlipUVs
                              : aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights;

        Assimp::Importer importer{};
        const auto* scene = importer.ReadFile(filename, flags);

        if(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene || !scene->mRootNode)
        {
            LOG_ERROR(MeshManager, std::string("failed to load file\nerror string: ") + importer.GetErrorString());
        }

        assert(scene->mNumMeshes == 1 && "can only load one mesh from file at the time");

        return meshes_.emplace_back(process_mesh(scene)).get();
    }

    Mesh* MeshManager::load(const std::string& animation_filename,
                            const std::string& mesh_filename,
                            bool flip_uvs,
                            std::function<void(const std::vector<Animation>&)> animation_callback)
    {
        auto flags = flip_uvs ? aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights | aiProcess_FlipUVs
                              : aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights;

        Assimp::Importer animation_importer{};
        Assimp::Importer mesh_importer{};

        const auto* mesh_scene = mesh_importer.ReadFile(mesh_filename, flags);
        const auto* animation_scene = animation_importer.ReadFile(animation_filename, flags);
    
        if(mesh_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !mesh_scene || !mesh_scene->mRootNode)
        {
            LOG_ERROR(MeshManager, std::string("failed to load file mesh\nerror string: ") + mesh_importer.GetErrorString());
        }
        else if(animation_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !animation_scene || !animation_scene->mRootNode)
        {
            LOG_ERROR(MeshManager, std::string("failed to load file animation\nerror string: ") + animation_importer.GetErrorString());
        }

        assert(mesh_scene->mNumMeshes == 1 && "can only load one mesh from file at the time");
        animation_callback(process_animations(animation_scene));

        return meshes_.emplace_back(process_mesh(mesh_scene)).get();
    }

    Mesh* MeshManager::mesh(std::uint32_t index)
    {
        return meshes_.at(index).get();
    }

    void MeshManager::remove(std::uint32_t index)
    {
        meshes_.erase(std::begin(meshes_) + index);
    }
}