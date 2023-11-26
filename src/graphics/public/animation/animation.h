#pragma once

#include "keyframe.h"

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

namespace muse
{

enum class PlaybackType : std::uint8_t
{
    SINGLE, // Play animation single time
    LOOP    // Loop animation until isn't stopped by user
};

/**
 *
 *  Class that's responsible for skeleton and needs to update it every frame
 *  and advance in animation.
 *
 */
class Animation
{
  public:
    /**
     *
     *  Create an animation.
     *
     *  @param name Name of animation.
     *  @param duration Duration of animation.
     *  @param frames Map that maps every bone to it's series of keyframes.
     *
     */
    Animation(std::string_view name,
              std::chrono::milliseconds duration,
              const std::unordered_map<std::string, std::vector<Keyframe>> &frames);

    /**
     *
     *  Get name of animation.
     *
     *  @return Name string.
     *
     */
    std::string name() const;

    /**
     *
     *  Advances in time by "ticks_per_second".
     *
     *  @param delta_time Delta.
     *
     */
    void advance();

    /**
     *
     *  Get current time.
     *
     *  @return Time.
     *
     */
    std::chrono::milliseconds time() const;

    /**
     *
     *  Get duration.
     *
     *  @return Duration.
     *
     */
    std::chrono::milliseconds duration() const;

    /**
     *
     *  Set current time.
     *
     *  @param time New time.
     *
     */
    void set_time(std::chrono::milliseconds time);

    /**
     *
     *  Get if the frames for given bone exist.
     *
     *  @param name Name of bone.
     *
     *  @return True if it has it's own frames, false otherwise.
     *
     */
    bool frames_exist(const std::string &name) const;

    /**
     *
     *  Query for transform at current time if it falls between, interpolate.
     *
     *  @param name Name of bone to query for.
     *
     *  @return Transformation.
     *
     */
    Transform transform(const std::string &name);

    /**
     *
     *  Get if animation is running currently.
     *
     *  @return True if it is, false otherwise.
     *
     */
    bool running() const;

    /**
     *
     *  Reset to start.
     *
     */
    void reset();

    /**
     *
     *  Set playback type of animation.
     *
     *  @param playback_type New playback type.
     *
     */
    void set_playback_type(PlaybackType playback_type);

    /**
     *
     *  Get playback type of animation.
     *
     *  @return Playback type.
     *
     */
    PlaybackType playback_type() const;

  private:
    /** Name of animation. */
    std::string name_;

    /** Current time. */
    std::chrono::milliseconds time_;

    /** Duration of animation. */
    std::chrono::milliseconds duration_;

    /** Frame map. */
    std::unordered_map<std::string, std::vector<Keyframe>> frames_;

    /** Animation's playback. */
    PlaybackType playback_type_;

    /** Last. */
    std::chrono::steady_clock::time_point last_;
};

}