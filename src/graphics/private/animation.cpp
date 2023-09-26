#include "graphics/public/animation.h"

#include <cassert>

namespace muse
{
    Animation::Animation(std::string_view name,
                         float duration,
                         float ticks_per_second,
                         const std::unordered_map<std::string, std::vector<Keyframe>>& frames)
        : name_(name)
        , time_(0.0f)
        , duration_(duration)
        , ticks_per_second_(ticks_per_second)
        , frames_(frames)
    {
    }

    void Animation::advance(float delta_time)
    {
        if(running())
        {
            // advance by ticks per second
            time_ += ticks_per_second_ * delta_time;
        
            if(playback_type_ == PlaybackType::LOOP)
            {
                // wrap back around if animation playback type is looping   
                time_ = std::fmod(time_, duration_);
            }
        }
    }

    float Animation::time() const
    {
        return time_;
    }

    float Animation::ticks_per_second() const
    {
        return ticks_per_second_;
    }

    float Animation::duration() const
    {
        return duration_;
    }

    void Animation::set_time(float time)
    {
        time_ = time;
    }

    bool Animation::frames_exist(const std::string& name) const
    {
        return frames_.contains(name);
    }

    Transform Animation::transform(const std::string& name)
    {
        const auto it = frames_.find(name);
        assert(it != std::cend(frames_) && "bone doesn't have it's own set of frames");

        const auto& keyframes = it->second;

        auto end_frame = std::find_if(std::cbegin(keyframes) + 1, std::cend(keyframes), [&](const Keyframe& keyframe){
            return keyframe.time > time_;
        });     

        // If frame goes out of bounds walk back frame
        if(end_frame == std::cend(keyframes))
        {
            --end_frame;
        }

        const auto start_frame = --end_frame;

        const auto frames_diff = end_frame->time - start_frame->time;
        const auto midway_length = time_ - end_frame->time;

        const auto interpolation = midway_length / frames_diff;
        
        const auto transform = start_frame->transform.interpolate(end_frame->transform, interpolation);
        return transform;
    }

    bool Animation::running() const
    {
        return playback_type_ == PlaybackType::LOOP ? true : time_ >= duration_;  
    }

    void Animation::reset()
    {
        time_ = 0.0f;
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