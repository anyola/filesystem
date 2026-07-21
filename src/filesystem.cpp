#include "filesystem.hpp"
#include "directory.hpp"

#include <vector>

namespace filesystem{
    fsobject::fsobject(std::string name_) : name(std::move(name_)) {}

    std::string fsobject::get_name() const {
        return name;
    }
    fsobject* fsobject::get_parent() const {
        return parent;
    }
    std::string fsobject::get_path() {
        std::vector<fsobject*> current_path;
        fsobject* current = this;
        while(current != nullptr){
            current_path.push_back(current);
            current = current->get_parent();
        }
        std::string result = "";
        while(!current_path.empty()){
            result += "/";
            result += current_path.back()->get_name();
            current_path.pop_back();
        }
        return result;
    }
    void fsobject::set_parent(fsobject* new_parent) {
        parent = new_parent;
    }
    bool fsobject::is_valid_name(const std::string& new_name){
        if (new_name.empty()) {
            return false;
        }
        if (new_name == "." || new_name == "..") {
            return false;
        }
        if (new_name.find('/') != std::string::npos) {
            return false;
        }
        return true;
    }
    bool fsobject::rename(const std::string& new_name) {
        if(is_valid_name(new_name)){
            directory* parent_dir = static_cast<directory*>(get_parent());
            if(parent_dir && parent_dir->validate_name(new_name)){
                name = new_name;
                return true;
            }
        }
        return false;
    }
    
}

