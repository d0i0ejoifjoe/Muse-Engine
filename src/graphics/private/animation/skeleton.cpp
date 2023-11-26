#include "graphics/public/animation/skeleton.h"

#include "graphics/public/animation/animation.h"
#include "log/public/logger.h"

void update_helper(muse::Animation *animation,
                   const std::vector<muse::Bone> &bones,
                   const std::vector<std::size_t> &parents,
                   std::vector<muse::Matrix4> &transforms)
{
    muse::Matrix4 root_inverse = muse::Matrix4::inverse(bones.front().transform);

    std::vector<muse::Matrix4> cache(transforms.size());
    transforms[0] = bones.front().transform;

    for (auto i = 1u; i < bones.size(); i++)
    {
        auto &bone = bones[i];

        if (animation->frames_exist(bone.name))
        {
            cache[i] = cache[parents[i]] * animation->transform(bone.name).matrix();
            transforms[i] = root_inverse * cache[i] * bone.offset;
        }
        else
        {
            cache[i] = cache[parents[i]] * bone.transform;
            transforms[i] = root_inverse * cache[i] * bone.offset;
        }
    }
}

namespace muse
{

Skeleton::Skeleton(const std::vector<Bone> &bones,
                   const std::vector<std::size_t> &parents)
    : bones_(bones)
    , parents_(parents)
    , transforms_(100, Matrix4{1.0f})
    , animation_(nullptr)
{
}

Skeleton::Skeleton(const std::vector<Bone> &bones,
                   const std::vector<std::size_t> &parents,
                   Animation *animation)
    : bones_(bones)
    , parents_(parents)
    , transforms_(100, Matrix4{1.0f})
    , animation_(animation)
{
}

const std::vector<Bone> &Skeleton::bones() const
{
    return bones_;
}

const std::vector<Matrix4> &Skeleton::transforms() const
{
    return transforms_;
}

Animation *Skeleton::animation() const
{
    return animation_;
}

void Skeleton::update()
{
    assert(animation_ != nullptr && "Animation cannot be null");
    update_helper(animation_, bones_, parents_, transforms_);
}

Bone *Skeleton::find_bone(std::string_view name)
{
    auto it = std::find_if(std::begin(bones_), std::end(bones_), [&](const Bone &bone)
                           {
                               return bone.name == name;
                           });

    if (it != std::end(bones_))
    {
        return std::to_address(it);
    }

    return nullptr;
}

const std::vector<std::size_t> &Skeleton::parents() const
{
    return parents_;
}

void Skeleton::set_animation(Animation *animation)
{
    animation_ = animation;
}

}