#include "log/public/logger.h"

#include <fmt/color.h>
#include <fstream>

namespace muse
{
    Logger::Logger()
        : formatter_(std::make_unique<Formatter>())
    {
    }

    Logger::~Logger() = default;

    Logger& Logger::instance()
    {
        static Logger logger{};
        return logger;
    }

    void Logger::log(LogLevel level,        
                     const std::string& category,
                     const std::string& filename,
                     std::uint32_t line,
                     const std::string& log_file,
                     const std::string& message)
    {
        auto format_str = formatter_->format(level, message, category, filename, line);
        format_str += '\n';
        
        if(!log_file.empty())
        {
            std::ofstream file{log_file, std::ios::app};
            file << format_str;
        }
        else
        {
            fmt::color color = fmt::color::white;

            switch(level)
            {
            case LogLevel::INFO: color = fmt::color::white; break;
            case LogLevel::WARN: color = fmt::color::yellow; break;
            case LogLevel::ERR: color = fmt::color::red; break;
            }

            fmt::print(fmt::fg(color), format_str);
        }
    }
}