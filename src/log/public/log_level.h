#pragma once

#include <cstdint>
#include <ostream>

namespace muse
{

/** Enum for different log levels */
enum class LogLevel : std::uint8_t
{
    INFO, // Information
    WARN, // Warning
    ERR   // Error
};

/**
 *
 *  Write log level to stream.
 *
 *  @param os Stream to write to.
 *  @param level Level to write.
 *
 *  @return Reference to stream.
 *
 */
inline std::ostream &operator<<(std::ostream &os, LogLevel level)
{
    switch (level)
    {
        case LogLevel::INFO: os << "Info"; break;
        case LogLevel::WARN: os << "Warning"; break;
        case LogLevel::ERR: os << "Error"; break;
    }

    return os;
}

}