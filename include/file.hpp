#ifndef FILE_HPP
#define FILE_HPP

#include "filesystem.hpp"
#include <cstddef>

namespace filesystem{
    class file final: public fsobject {
    private:
        std::string content;
    public:
        explicit file(std::string name_, std::string content_ =  "");
        
        [[nodiscard]] std::size_t get_size() const override;
        [[nodiscard]] bool is_directory() const override;
        [[nodiscard]] std::unique_ptr<fsobject> clone() const override;
        void print(std::ostream& os) const override;

        [[nodiscard]] const std::string& get_content() const;
        void set_content(std::string data);
    };
}

#endif