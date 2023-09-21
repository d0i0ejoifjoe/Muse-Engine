#include "graphics/public/skeleton.h"

#include <algorithm>

/**
 * 
 *  Because of assimp nature of node hierarchy and that aiNodeAnim
 *  has it name string of bone that it is applied to, we can check if bone is in our bone hierarchy
 *  and if it is we can calculate aiNodeAnim transformation in current time of animation and apply it to bone,
 *  else we apply the transform that is in this structure and then we traverse the "children" vector and do this
 *  again and again until we end up with final transform that we add at position of bone in transforms_ vector in skeleton
 *  and we need to multiply it with offset matrix of bone.
 * 
*/
struct AssimpNode
{
    /** Transformation of node */
    glm::mat4 transform;

    /** Name of bone */
    std::string name;

    /** All of children (if node is single without vector means it is root node) */
    std::vector<AssimpNode> children;
};

/**
 * 
 *  Recursive helper function to update transformations.
 * 
 *  @param node Const pointer to node in assimp heirarchy.
 *  @param skeleton Pointer to skeleton (this).
 *  @param parent_transform Parent transformation.
 *  @param root_inverse_transform Inverse root transform matrix.
 *  @param transforms Reference to transforms vector.
 *  @param time Current time.
 * 
*/
void update_transforms(const AssimpNode* node,
                       muse::Skeleton* skeleton,
                       std::vector<glm::mat4>& transforms,
                       glm::mat4& parent_transform,
                       const glm::mat4& root_inverse_transform,
                       std::chrono::milliseconds time)
{
    std::string name = node->name;
    glm::mat4 node_transform = node->transform;

    auto* bone = skeleton->find_bone(name);
    if(bone)
    {
        node_transform = skeleton->transform(name, time).matrix();
    }

    glm::mat4 global_transform = parent_transform * node_transform; 

    if(bone)
    {
        auto index = bone->index;
        transforms[index] = root_inverse_transform * global_transform * bone->offset; 
    }

    for(auto i = 0u; i < node->children.size(); i++)
    {
        update_transforms(&node->children[i], skeleton, transforms, global_transform, root_inverse_transform, time);
    }
}

namespace muse
{
    Skeleton::Skeleton(const std::vector<Bone>& bones,
                       const AssimpNode& root,
                       const std::unordered_map<std::string, std::vector<Keyframe>>& frames)
        : bones_(bones)
        , root_(std::addressof(root))
        , transforms_(100)
        , frames_(frames)
    {
        auto counter = 0u;

        // Setup indices to then find bone transforms in vertex shader.
        for(auto& bone : bones_)
        {
            bone.index = counter;
            counter++;
        }
    }

    void Skeleton::update(std::chrono::milliseconds time)
    {
        glm::mat4 identity{1.0f};
        const auto inverse = glm::inverse(root_->transform);

        update_transforms(root_, this, transforms_, identity, inverse, time);
    }

    const std::vector<Bone>& Skeleton::bones() const
    {
        return bones_;
    }

    const std::vector<glm::mat4>& Skeleton::transforms() const
    {
        return transforms_;
    }

    Bone* Skeleton::find_bone(std::string_view name)
    {
        auto it = std::find_if(std::begin(bones_), std::end(bones_), [&](const auto& bone)
        {
            return bone.name == name;
        });

        if(it == std::cend(bones_))
        {
            return nullptr;
        }

        return std::to_address(it);
    }

    Transform Skeleton::transform(const std::string& name, std::chrono::milliseconds time)
    {
        const auto& keyframes = frames_.find(name)->second;

        // find frame after current time because of it add 1 to begin iterator
        auto end_frame = std::find_if(std::cbegin(keyframes) + 1, std::cend(keyframes), [&](const auto& keyframe){
            return keyframe.time > time;
        });

        if(end_frame == std::cend(keyframes))
        {
            // past the last frame so use last frame.
            end_frame--;
        }

        // get starting frame means go back a frame
        auto start_frame = --end_frame;

        // calculate how much we need to intepolate
        const auto midway_length = time - start_frame->time;
        const auto frames_diff = end_frame->time - start_frame->time;
        const auto interpolation = static_cast<float>(midway_length.count()) / static_cast<float>(frames_diff.count());

        // get start frame transform and interpolate.
        const auto transform = start_frame->transform;
        return transform.interpolate(end_frame->transform, interpolation);
    }
}