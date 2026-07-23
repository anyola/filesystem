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

        [[nodiscard]] std::size_t get_size() const override;
        [[nodiscard]] bool is_directory() const override;
        [[nodiscard]] bool is_symlink() const override;
        [[nodiscard]] std::unique_ptr<fsobject> clone() const override;
        void print(std::ostream& os) const override;

        std::vector<fsobject*> find_all();
        [[nodiscard]] std::vector<fsobject*> find_all_current_level() const ;
        fsobject* find(const std::string& input_name);
        fsobject* find_by_path(std::string input_name);

        bool has_child(const std::string& child_name);
        fsobject* add_child(std::unique_ptr<fsobject> object);
        std::unique_ptr<fsobject> remove_child(const std::string& child_name);
        bool is_ancestor(fsobject& object);
        fsobject* move(const std::string& name, directory& target);
        [[nodiscard]] bool validate_name(const std::string& name) const;
    };
}

#endif