#ifndef SYMLINK_HPP
#define SYMLINK_HPP

#include "filesystem.hpp"
#include <cstddef>
#include <memory>
#include <ostream>
#include <unordered_set>

namespace filesystem{
    class symlink final: public fsobject {
    private:
        std::string target_path;
    public:
        explicit symlink(std::string name_, std::string target_path);
        
        [[nodiscard]] std::size_t get_size() const override;
        [[nodiscard]] bool is_directory() const override;
        [[nodiscard]] bool is_symlink() const override;
        [[nodiscard]] std::unique_ptr<fsobject> clone() const override;
        void print(std::ostream& os) const override;

        const std::string& get_target_path() const;
        void set_target_path(std::string new_target);
        fsobject* resolve(int max_depth = 10);
    };
}

#endif