#ifndef DIRECTORY_HPP
#define DIRECTORY_HPP

#include "filesystem.hpp"
#include <vector>
#include <memory>

namespace filesystem{
    class directory final : public fsobject{
    private:
        std::vector<std::unique_ptr<fsobject>> children;
    public:
        explicit directory(std::string name_);

        std::size_t get_size() const override;
        bool is_directory() const override;
        std::unique_ptr<fsobject> clone() override;

        std::vector<fsobject*> find_all();
        std::vector<fsobject*> find_all_current_level();
        fsobject* find(const std::string& input_name);

        bool has_child(const std::string& child_name);
        fsobject* add_child(std::unique_ptr<fsobject> object);
        std::unique_ptr<fsobject> remove_child(const std::string& child_name);
        bool is_ancestor(fsobject& object);
        fsobject* move(const std::string& name, directory& target);
        bool validate_name(const std::string& name);
    };
}


#endif