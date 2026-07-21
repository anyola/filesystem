#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>
#include <memory>

namespace filesystem {
    class fsobject{
    private:
        std::string name;
        fsobject* parent = nullptr;
        void set_parent(fsobject* new_parent); 
        friend class directory;
    public:
        explicit fsobject(std::string name_);
        fsobject(const fsobject&) = delete;
        fsobject& operator=(const fsobject&) = delete;
        fsobject(fsobject&&) = delete;
        fsobject& operator=(fsobject&&) = delete;
        virtual ~fsobject() = default;

        std::string get_name() const;
        std::string get_path();
        fsobject* get_parent() const;
        bool rename(const std::string& new_name);

        virtual std::size_t get_size() const = 0;
        virtual bool is_directory() const = 0;
        virtual std::unique_ptr<fsobject> clone() = 0;
        static bool is_valid_name(const std::string& new_name);
    };
}

#endif