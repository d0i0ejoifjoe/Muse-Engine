#include "graphics/public/animation/animation.h"

#include "log/public/logger.h"
#include "utils/public/utils.h"

#include <cassert>
#include <sys/time.h>
namespace muse
{

Animation::Animation(std::string_view name,
                     std::chrono::milliseconds duration,
                     const std::unordered_map<std::string, std::vector<Keyframe>> &frames)
    : name_(name)
    , time_(duration)
    , duration_(duration)
    , frames_(frames)
    , playback_type_(PlaybackType::LOOP)
    , last_(std::chrono::steady_clock::now())
{
}

std::string Animation::name() const
{
    return name_;
}

void Animation::advance()
{
    if (running())
    {
        const auto now = std::chrono::steady_clock::now();
        const auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_);

        // advance by delta
        time_ += delta;

        if (playback_type_ == PlaybackType::LOOP)
        {
            // wrap back around if animation playback type is looping
            time_ = time_ % duration_;
        }

        last_ = now;
    }
}

std::chrono::milliseconds Animation::time() const
{
    return time_;
}

std::chrono::milliseconds Animation::duration() const
{
    return duration_;
}

void Animation::set_time(std::chrono::milliseconds time)
{
    time_ = time;
}

bool Animation::frames_exist(const std::string &name) const
{
    return frames_.contains(name);
}

Transform Animation::transform(const std::string &name)
{
    const auto it = frames_.find(name);
    assert(it != std::cend(frames_) && "bone doesn't have it's own set of frames");

    const auto &keyframes = it->second;

    auto end_frame = std::find_if(std::cbegin(keyframes) + 1, std::cend(keyframes),
                                  [&](const Keyframe &keyframe)
                                  {
                                      return keyframe.time >= time_;
                                  });

    // If frame goes out of bounds walk back frame
    if (end_frame == std::cend(keyframes))
    {
        --end_frame;
    }

    // note we subtracting one not decrementiting because
    // we want to get new iterator before end frame
    const auto start_frame = end_frame - 1;

    const auto frames_diff = end_frame->time - start_frame->time;
    const auto midway_length = time_ - start_frame->time;

    const auto alpha = static_cast<float>(midway_length.count()) / static_cast<float>(frames_diff.count());

    const auto transform = start_frame->transform.interpolate(end_frame->transform, alpha);

    return transform;
}

bool Animation::running() const
{
    return playback_type_ == PlaybackType::LOOP ? true : time_ < duration_;
}

void Animation::reset()
{
    using namespace std::chrono_literals;

    time_ = 0ms;
    last_ = std::chrono::steady_clock::now();
}

void Animation::set_playback_type(PlaybackType playback_type)
{
    playback_type_ = playback_type;
}

PlaybackType Animation::playback_type() const
{
    return playback_type_;
}

}