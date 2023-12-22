#pragma once

#include "utils.h"

#include <filesystem>
#include <unordered_map>

namespace muse
{

/**
 *
 *  A singleton that is used to load and manage the file data, for example model data,
 *  image data, etc.
 *
 */
class FileManager
{
  public:
    /**
     *
     *  Get the instance of file manager.
     *
     */
    static FileManager &instance();

    /**
     *
     *  Set root directory.
     *
     *  @param root New root directory.
     *
     */
    void set_root_directory(std::filesystem::path root);

    /**
     *
     *  Get root directory.
     *
     *  @return Root.
     *
     */
    std::filesystem::path root_directory() const;

    /**
     *
     *  Load file.
     *
     *  @param filename Path to the file (relative to file's manager root).
     *  @param name Name for data to then find it in map.
     *
     *  @return Reference to loaded data (empty if loading failed).
     *
     */
    const Data &load(std::string_view filename, const std::string &name);

    /**
     *
     *  Remove loaded file.
     *
     *  @param name Name of data to remove.
     *
     */
    void remove(const std::string &name);

    /**
     *
     *  Query for certain data using the name.
     *
     *  @param name Name of data.
     *
     */
    const Data &operator[](const std::string &name);

    /**
     *
     *  Get reference to map of resources.
     *
     *  @return Reference to resource map.
     *
     */
    const std::unordered_map<std::string, Data> &resources() const;

  private:
    /**
     *
     *  Create a file manager.
     *
     */
    FileManager();

    /** Map of name of resource and it's data. */
    std::unordered_map<std::string, Data> resources_;

    /** Root. */
    std::filesystem::path root_;
};
}