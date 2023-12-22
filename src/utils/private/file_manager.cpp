#include "utils/public/file_manager.h"

#include "log/public/logger.h"

#include <cassert>
#include <fstream>

namespace muse
{

FileManager::FileManager()
    : resources_()
    , root_(".")
{
    std::filesystem::path current_path = std::filesystem::current_path();
    root_ = current_path.root_directory();
    LOG_INFO(FileManager, "File manager created!");
}

FileManager &FileManager::instance()
{
    static FileManager manager{};
    return manager;
}

void FileManager::set_root_directory(std::filesystem::path root)
{
    root_ = root;
}

const Data &FileManager::load(std::string_view filename, const std::string &name)
{
    std::fstream file{root_ / filename, std::ios::binary | std::ios::in};

    if (!file)
    {
        LOG_ERROR(FileManager, "Failed to open file\nName of resource: {}\nPath: {}", name, (root_ / filename).native());
    }

    std::stringstream strm{};
    strm << file.rdbuf();

    const auto data = strm.str();
    const auto *data_ptr = reinterpret_cast<const std::byte *>(data.data());
    const auto size = data.length();

    Data data_buffer{data_ptr, data_ptr + size};

    return resources_[name] = std::move(data_buffer);
}

std::filesystem::path FileManager::root_directory() const
{
    return root_;
}

void FileManager::remove(const std::string &name)
{
    resources_.erase(name);
}

const Data &FileManager::operator[](const std::string &name)
{
    return resources_[name];
}

const std::unordered_map<std::string, Data> &FileManager::resources() const
{
    return resources_;
}

}