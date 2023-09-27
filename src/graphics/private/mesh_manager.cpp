#include "graphics/public/mesh_manager.h"
#include "graphics/public/mesh.h"
#include "graphics/public/vertex.h"
#include "graphics/public/skeleton.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cstdint>
#include <unordered_map>

struct Node
{
    std::string name;
    glm::mat4 transform;
}

glm::mat4 convert_matrix(const aiMatrix4x4& m)
{
    return glm::mat4{m.a1, m.a2, m.a3,
                     m.b1, m.b2, m.b3,
                     m.c1, m.c2, m.c3};
}

glm::vec3 convert_vec(const aiVector3D& vec)
{
    return glm::vec3{vec.x, vec.y, vec.z};
}

glm::vec4 convert_vec(const aiColor4D& vec)
{
    return glm::vec3{vec.x, vec.y, vec.z, vec.w};
}

glm::vec2 convert_vec(const aiVector2D& vec)
{
    return glm::vec2{vec.x, vec.y};
}

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
            texture_coordinates = convert_vec(mesh->mColors[0][i]);
        }

        if(mesh->HasTextureCoords(0))
        {
            color = convert_vec(mesh->mTextureCoords[0][i]);
        }

        const auto tangent = convert_vec(mesh->mTangents[i]);
        const auto bitangent = convert_vec(mesh->mBitangents[i]); 
    
        vertices.push_back(muse::Vertex{position, normal, color, texture_coordinates, tangent, bitangent});
    }

    return vertices;
}

const aiBone* find_bone(const aiString& name, const aiMesh* mesh)
{
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

std::vector<Bone> process_children(const aiBone* bone, const aiMesh* mesh)
{
    auto node = bone->node;

    if(node->mNumChildren == 0)
    {
        return {};
    }

    std::vector<Bone> children{};
    for(auto i = 0u; i < node->mNumChildren; i++)
    {
        auto children_node = node->mChildren[i];
        auto children_bone = find_bone(children_node->mName, mesh);
        
        children.push_back(muse::Bone{children_bone->mName.data,
                                     convert_matrix(children_bone->mOffsetMatrix),
                                     convert_matrix(children_node->mTransform),
                                     -1,
                                     process_children(children_bone, mesh)});
    }

    return children;
} 

/**
 * 
 *  Get root bone.
 * 
 *  @param scene Assimp scene.
 *  @param mesh Mesh with all bones.
 * 
*/
const aiBone* find_root_bone(const aiScene* scene, const aiMesh* mesh)
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
        auto node = mesh->mBones[i]->node;
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

void process_weights(std::vector<muse::Vertex>& vertices,
                     const aiMesh* mesh,
                     std::unordered_map<std::string, std::uint32_t>& bone_name_to_index)
{
    for(auto i = 0u; i < mesh->mNumBones; i++)
    {
        auto bone = mesh->mBones[i];

        for(auto k = 0u; k < bone->mNumWeights; k++)
        {
            auto weight = bone->mWeights[k];
            auto& vertex = vertices.at(weight.mVertexId);
            vertex.weights.push_back(muse::Weight{bone_name_to_index[bone->mName.data], weight.mWeight});  
        }
    }
}

void setup_bone_indices(std::unordered_map<std::string, std::uint32_t>& bone_name_to_index,
                        std::uint32_t& index,
                        const Bone& bone)
{
    bone_name_to_index[bone.name] = index;

    for(const auto& children_bone : bone.children)
    {
        setup_bone_indices(bone_name_to_index, ++index, children_bone);
    }
}

muse::Skeleton process_skeleton(std::vector<muse::Vertex>& vertices, const aiMesh* mesh, const aiScene* scene)
{
    std::vector<Bone> bones{};

    std::uint32_t bone_counter = 0u;
    std::unordered_map<std::string, std::uint32_t> bone_name_to_index{};

    // Get root bone.
    auto root = find_root_bone(scene, mesh);
    auto children = process_children(bone, mesh);

    muse::Bone root_bone{root->mName.data,
                         convert_matrix(root->mOffsetMatrix),
                         convert_matrix(root->node->mTransform),
                         -1,
                         children};

    setup_bone_indices(bone_name_to_index, bone_counter, root_bone);

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
}

muse::Mesh process_mesh(const aiMesh* mesh, const aiScene* scene)
{
    auto vertices = process_vertices(mesh);
    auto skeleton = process_skeleton(vertices, mesh, scene);
    auto indices = process_indices(mesh);
    
    muse::Mesh mesh{vertices, indices, skeleton};
    return mesh;
}

namespace muse
{
    Mesh* MeshManager::load(const std::string& filename,
                            bool flip_uvs,
                            std::function<void(const std::vector<Animation>&)> animation_callback)
    {
        auto flags = flip_uvs ? aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights | aiProcess_PopulateArmatureData | aiProcess_FlipUVs
                              : aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights | aiProcess_PopulateArmatureData;

        Assimp::Importer importer{};
        const auto* scene = importer.ReadFile(filename.c_str(), flags);
        
        assert(scene->mNumMeshes == 1 && "can only load one mesh from file at the time");
    
        meshes.push_back(std::make_unique<Mesh>(process_mesh(scene->mMeshes[0], scene)));
    }
}