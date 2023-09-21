#include "graphics/public/animation.h"

using namespace std::literals::chrono_literals;

namespace muse
{
    Animation::Animation(Skeleton* skeleton,
                         std::chrono::milliseconds duration,
                         PlaybackType playback_type,
                         std::string_view name)
        : skeleton_(skeleton)
        , duration_(duration)
        , time_(0ms)
        , running_(true)
        , playback_type_(playback_type)
        , name_(name)
    {
    }

    void Animation::set_playback_type(PlaybackType playback_type)
    {
        playback_type_ = playback_type;
    }

    PlaybackType Animation::playback_type() const
    {
        return playback_type_;
    }

    void Animation::update()
    {
        if(running_)
        {
            const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());
        
            // advance
            time_ += now;

            if(playback_type_ == PlaybackType::LOOP)
            {
                // wrap back if looping playback 
                time_ = time_ % duration_;
            }
            else if(playback_type_ == PlaybackType::SINGLE && time_ >= duration_)
            {
                // if single playback stop animation if time is equal or grater than duration of animation 
                stop();
            }

            skeleton_->update(time_);
        }
    }

    bool Animation::running() const
    {
        return running_;
    }

    void Animation::start()
    {
        running_ = true;
        time_ = 0ms;
    }

    void Animation::stop()
    {
        running_ = false;
        time_ = 0ms;
    }

    std::string Animation::name() const
    {
        return name_;
    }
}