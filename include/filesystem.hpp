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

        [[nodiscard]] std::string get_name() const;
        std::string get_path();
        [[nodiscard]] fsobject* get_parent() const;
        bool rename(const std::string& new_name);

        [[nodiscard]] virtual std::size_t get_size() const = 0;
        [[nodiscard]] virtual bool is_directory() const = 0;
        [[nodiscard]] virtual bool is_symlink() const = 0;
        virtual std::unique_ptr<fsobject> clone() const = 0;
        virtual void print(std::ostream& os) const = 0;
        static bool is_valid_name(const std::string& new_name);
    };
}

#endif