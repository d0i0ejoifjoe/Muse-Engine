#pragma once

#include "bone.h"

namespace muse
{

class Animation;

/**
 *
 *  Class that helps us update bones transformations
 *  and query for them.
 *
 */
class Skeleton
{
  public:
    /**
     *
     *  Create an empty skeleton.
     *
     */
    Skeleton() = default;

    /**
     *
     *  Create a skeleton that has no animation responsible for it.
     *
     *  @param bones Bones that are ordered in certain way.
     *  @param parents Parent indices.
     *
     */
    explicit Skeleton(const std::vector<Bone> &bones,
                      const std::vector<std::size_t> &parents);

    /**
     *
     *  Create a skeleton.
     *
     *  @param bones Bones that are ordered in certain way.
     *  @param parents Parent indices.
     *  @param animation Animation that's responsible for the skeleton.
     *
     */
    Skeleton(const std::vector<Bone> &bones,
             const std::vector<std::size_t> &parents,
             Animation *animation);

    /**
     *
     *  Get const reference to bones
     *
     *  @return Bones.
     *
     */
    const std::vector<Bone> &bones() const;

    /**
     * 
     *  Get parents indices.
     * 
     *  @return Parents.
     * 
    */
    const std::vector<std::size_t> &parents() const;

    /**
     *
     *  Get transforms.
     *
     *  @return Transforms.
     *
     */
    const std::vector<Matrix4> &transforms() const;

    /**
     *
     *  Get animation.
     *
     *  @return Pointer to animation.
     *
     */
    Animation *animation() const;

    /**
     *
     *  Update all bone transformations.
     *
     */
    void update();

    /**
     *
     *  Find bone by name.
     *
     *  @param name Name of bone.
     *
     *  @return Nullptr if bone doesn't exist, pointer to bone otherwise.
     *
     */
    Bone *find_bone(std::string_view name);

    /**
     *
     *  Set skeleton owning animation.
     *
     *  @param animation Pointer to animation.
     *
     */
    void set_animation(Animation *animation);

  private:
    /** Bones. */
    std::vector<Bone> bones_;

    /** Parents. */
    std::vector<std::size_t> parents_;

    /** Transformations. */
    std::vector<Matrix4> transforms_;

    /** Animation that's responsible for skeleton. */
    Animation *animation_;
};

}