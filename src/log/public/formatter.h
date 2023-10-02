#pragma once

#include "log_level.h"

namespace muse
{

/**
 *
 *  Class that is used by logger for formatting log entries.
 *
 */
class Formatter
{
  public:
    /**
     *
     *  Create formatter.
     *
     */
    Formatter();

    /**
     *
     *  Destroy formatter.
     *
     */
    ~Formatter();

    /**
     *
     *  Format a log entry.
     *
     *  @param level Level of log entry.
     *  @param message Text of log entry.
     *  @param category Category of log entry.
     *  @param filename Filename of log entry.
     *  @param line Line of log entry.
     *
     *  @return Formatted into string log entry.
     *
     */
    std::string format(LogLevel level, const std::string &message, const std::string &category,
                       const std::string &filename, std::uint32_t line);
};

} // namespace muse