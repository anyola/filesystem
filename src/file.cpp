#include "file.hpp"

namespace filesystem{
    file::file(std::string name_, std::string content_) : fsobject(std::move(name_)), content(std::move(content_)) {}

    std::size_t file::get_size() const {
        return content.size();
    }
    bool file::is_directory() const {
        return false;
    }
    const std::string& file::get_content() const {
        return content;
    }
    void file::set_content(std::string data) {
        content = std::move(data);
    }
    std::unique_ptr<fsobject> file::clone() const {
        return std::make_unique<file>(get_name(), get_content());
    }
}