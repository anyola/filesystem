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
        
        std::size_t get_size() const override;
        bool is_directory() const override;
        std::unique_ptr<fsobject> clone() const override;

        const std::string& get_content() const;
        void set_content(std::string data);
    };
}

#endif