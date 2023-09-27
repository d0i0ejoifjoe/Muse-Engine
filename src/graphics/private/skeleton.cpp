#include "graphics/public/skeleton.h"
#include "graphics/public/animation.h"

void update_helper(muse::Animation* animation,
                   std::vector<glm::mat4>& transforms,
                   const glm::mat4& root_inverse_transform,
                   muse::Bone* bone,
                   glm::mat4& parent_transform)
{
    assert(animation != nullptr && "animation pointer cannot be nullptr");

    glm::mat4 bone_transform = bone->transform; 

    if(animation->frames_exist(bone->name))
    {
        bone_transform = animation->transform(bone->name).matrix();        
    }

    glm::mat4 global_transform = parent_transform * bone_transform;
    
    transforms[bone->index] = root_inverse_transform * global_transform * bone->offset; 
    
    for(auto& children : bone->children)
    {
        update_helper(animation, transforms, root_inverse_transform, std::addressof(children), global_transform);
    }
}

void find_bone_helper(std::string_view name,
                      muse::Bone& bone,
                      muse::Bone* out_bone)
{
    if(bone.name == name)
    {
        out_bone = std::addressof(bone);
    }

    if(!bone.children.empty())
    {
        for(auto& children : bone.children)
        {
            find_bone_helper(name, children, out_bone);
        }
    }
}

namespace muse
{
    Skeleton::Skeleton(const Bone& root_bone)
        : root_bone_(root_bone)
        , transforms_(100)
        , animation_(nullptr)
    {
    }

    Skeleton::Skeleton(const Bone& root_bone,
                       Animation* animation)
        : root_bone_(root_bone)
        , transforms_(100)
        , animation_(animation)
    {
    }

    const Bone* Skeleton::root_bone() const
    {
        return std::addressof(root_bone_);
    }

    const std::vector<glm::mat4>& Skeleton::transforms() const
    {
        return transforms_;
    }

    Animation* Skeleton::animation() const
    {
        return animation_;
    }

    void Skeleton::update()
    {
        auto identity = glm::mat4{1.0f};
        const auto inverse = glm::inverse(root_bone_.transform);
    
        update_helper(animation_, transforms_, inverse, std::addressof(root_bone_), identity);
    }

    Bone* Skeleton::find_bone(std::string_view name)
    {
        Bone* bone = nullptr;
        find_bone_helper(name, root_bone_, bone);
        
        return bone;
    }

    void Skeleton::set_animation(Animation* animation)
    {
        animation_ = animation;
    }
}