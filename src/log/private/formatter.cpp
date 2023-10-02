#include "log/public/formatter.h"

#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>
#include <iomanip>
#include <sstream>

std::string format_filename(const std::string &filename)
{
    auto last = filename.find_last_of(std::filesystem::path::preferred_separator);
    return filename.substr(last + 1);
}

namespace muse
{

Formatter::Formatter() = default;

Formatter::~Formatter() = default;

std::string Formatter::format(LogLevel level,
                              const std::string &message,
                              const std::string &category,
                              const std::string &filename,
                              std::uint32_t line)
{
    auto now = std::time(0);

    std::stringstream strm{};
    strm << std::put_time(std::localtime(&now), "%T") << ' ' << level << ' ' << '[' << category << '/' << format_filename(filename) << ':' << line << "]: " << message;

    return strm.str();
}

}