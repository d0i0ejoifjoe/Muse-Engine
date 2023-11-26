#include "log/public/logger.h"

#include <fmt/color.h>
#include <fstream>
#include <iostream>

namespace muse
{

Logger::Logger()
    : formatter_(std::make_unique<Formatter>())
{
}

Logger::~Logger() = default;

Logger &Logger::instance()
{
    static Logger logger{};
    return logger;
}

void Logger::log(LogLevel level,
                 const std::string &category,
                 const std::string &filename,
                 std::uint32_t line,
                 const std::string &log_file,
                 const std::string &message)
{
    auto format_str = formatter_->format(level, message, category, filename, line);
    format_str += '\n';

    if (!log_file.empty())
    {
        // we don't want to open new file everytime
        // because it is expensive, instead we can
        // record last opened file path
        // and compare it with current file path
        // if they match we won't open new file
        // like this we won't open new files
        // until the new path will be given
        static std::ofstream file{log_file, std::ios::app};
        static std::string previous_path = log_file;
        if (previous_path != log_file)
        {
            file = std::move(std::ofstream(log_file, std::ios::app));
        }

        file << format_str;
    }
    else
    {
        std::string color{"\x1B[0;0m"};
        bool is_err = false;

        switch (level)
        {
            case LogLevel::INFO:
                color = "\x1B[0;37m";
                break;
            case LogLevel::WARN:
                color = "\x1B[0;33m";
                break;
            case LogLevel::ERR:
                color = "\x1B[0;31m";
                is_err = true;
                break;
        }

        if (!is_err)
        {
            std::cout << color << format_str << "\x1B[0;0m";
        }
        else
        {
            std::cerr << color << format_str << "\x1B[0;0m";
        }
    }
}
}