#pragma once

#include "event.h"

#include <functional>
#include <optional>

namespace muse
{

/** Alias for event callback. */
using EventCallback = std::function<bool(const Event &)>;

/**
 *
 *  Handles the heavy lifting of polling for events and dispatching them
 *  to appropriate callbacks that are supplied by user.
 *
 */
class EventHandler
{
  public:
    /**
     *
     *  Create a handler.
     *
     */
    EventHandler();

    /**
     *
     *  Poll for events, add them to queue and
     *  then dispatch them by one to every callback.
     *
     */
    void dispatch();

    /**
     *
     *  Add event callback.
     *
     *  @param callback Event callback.
     *
     */
    void add_callback(EventCallback callback);

    /**
     *
     *  Remove callback.
     *
     *  @param index Index of callback.
     *
     */
    void remove_callback(std::uint32_t index);

  private:
    /** Callbacks. */
    std::vector<EventCallback> callbacks_;
};
}