#pragma once

#include "skeleton.h"

namespace muse
{
    /** Enum for different playback */
    enum class PlaybackType : std::uint8_t  
    {
        SINGLE, // Play animation single time
        LOOP // Loop animation until stopped by user
    };

    /**
     * 
     *  This class is used for managing time, advancing in animation, checking if it ended and if it is restart animation,
     *  and update a skeleton every frame.
     * 
    */
    class Animation
    {
    public:
        /**
         * 
         *  Create a animation.
         * 
         *  @param skeleton Skeleton to be updated.
         *  @param duration Duration of animation.
         *  @param playback_type Playback type.
         *  @param name Name of animation
         * 
        */
        Animation(Skeleton* skeleton,
                  std::chrono::milliseconds duration,
                  PlaybackType playback_type,
                  std::string_view name);

        /**
         * 
         *  Set playback type.
         * 
         *  @param playback_type New playback type.
         * 
        */
        void set_playback_type(PlaybackType playback_type);

        /**
         * 
         *  Get playback type.
         * 
         *  @return Current playback type.
         * 
        */
        PlaybackType playback_type() const;

        /**
         * 
         *  Update function.
         *  Should be called every frame.
         * 
        */
        void update();

        /**
         * 
         *  Get if animation is running.
         * 
         *  @return True if it is running, otherwise false.
         * 
        */
        bool running() const;

        /**
         * 
         *  Start animation.
         * 
        */
        void start();

        /**
         * 
         *  Stop animation.
         * 
        */
        void stop();

        /**
         * 
         *  Get name of animation.
         * 
         *  @return Name string.
         * 
        */
        std::string name() const;

    private:
        /** Skeleton. */
        Skeleton* skeleton_;
        
        /** Duration of animation. */
        std::chrono::milliseconds duration_;

        /** Time in animation. */
        std::chrono::milliseconds time_;

        /** Whether animation is running. */
        bool running_;

        /** Playback type. */
        PlaybackType playback_type_;      

        /** Name of animation. */
        std::string name_;  
    };
}