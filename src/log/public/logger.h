#pragma once

#include "formatter.h"
#include "format.h"

#include <memory>
#include <optional>
#include <fmt/core.h>

// TODO: Refactor

namespace muse
{
    /**
     * 
     *  Class that is used for logging log entries to a file or console.
     * 
    */
    class Logger
    {
    public:
        /**
         * 
         *  Get instance of logger.
         * 
         *  @return Instance.
         * 
        */
        static Logger& instance();

        /**
         * 
         *  Destroy logger.
         * 
        */
        ~Logger();

        /**
         * 
         *  Log message to console.
         * 
         *  @param level Log level.
         *  @param message Text of entry.
         *  @param category Category of entry.
         *  @param filename Filename.
         *  @param line Line.
         *  @param log_file Path to a log file if empty logging to console.
         * 
        */
        void log(LogLevel level,        
                 const std::string& category,
                 const std::string& filename,
                 std::uint32_t line,
                 const std::string& log_file,
                 const std::string& message);

        /**
         * 
         *  Log message to console.
         * 
         *  @param level Log level.
         *  @param message Text of entry.
         *  @param category Category of entry.
         *  @param filename Filename.
         *  @param line Line.
         *  @param log_file Path to a log file if empty logging to console.
         *  @param args Format args.
         * 
        */
        template<class... Args>
        void log(LogLevel level,        
                 const std::string& category,
                 const std::string& filename,
                 std::uint32_t line,
                 const std::string& log_file,
                 const std::string& message,
                 Args&& ...args)
        {
            auto format_message = format(message, std::forward<Args>(args)...);

            log(level, category, filename, line, log_file, format_message);
        }

    private:
        /**
         * 
         *  Create a logger.
         * 
        */
        Logger();

        /** Formatter */
        std::unique_ptr<Formatter> formatter_;
    };

    /** Log defines */
    #if !defined(NDEBUG)

    #define LOG_INFO(Category, ...) muse::Logger::instance().log(muse::LogLevel::INFO, #Category, __FILE__, __LINE__, "", ##__VA_ARGS__)
    #define LOG_WARN(Category, ...) muse::Logger::instance().log(muse::LogLevel::WARN, #Category, __FILE__, __LINE__, "", ##__VA_ARGS__)
    #define LOG_ERROR(Category, ...) muse::Logger::instance().log(muse::LogLevel::ERR, #Category, __FILE__, __LINE__, "", ##__VA_ARGS__)

    #define LOG_INFO_FILE(Category, FilePath, ...) muse::Logger::instance().log(muse::LogLevel::INFO, #Category, __FILE__, __LINE__, FilePath, ##__VA_ARGS__)
    #define LOG_WARN_FILE(Category, FilePath, ...) muse::Logger::instance().log(muse::LogLevel::WARN, #Category, __FILE__, __LINE__, FilePath, ##__VA_ARGS__)
    #define LOG_ERROR_FILE(Category, FilePath, ...) muse::Logger::instance().log(muse::LogLevel::ERR, #Category, __FILE__, __LINE__, FilePath, ##__VA_ARGS__)

    #else

    #define LOG_INFO(Category, ...)
    #define LOG_WARN(Category, ...)
    #define LOG_ERROR(Category, ...)

    #define LOG_INFO_FILE(Category, FilePath, ...)
    #define LOG_WARN_FILE(Category, FilePath, ...)
    #define LOG_ERROR_FILE(Category, FilePath, ...)

    #endif
}