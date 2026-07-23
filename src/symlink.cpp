#include "symlink.hpp"
#include "directory.hpp"

namespace filesystem{
    symlink::symlink(std::string name_, std::string target_path) : fsobject(std::move(name_)), target_path(std::move(target_path)) {}

    std::size_t symlink::get_size() const {
        return target_path.size();
    }
    bool symlink::is_directory() const {
        return false;
    }
    bool symlink::is_symlink() const {
        return true;
    }
    std::unique_ptr<fsobject> symlink::clone() const {
        return std::make_unique<symlink>(get_name(), target_path);
    }
    void symlink::print(std::ostream& os) const {
        os << get_name() << "->" << target_path << "(" << get_size() << " bytes)" << '\n';
    }
    const std::string& symlink::get_target_path() const {
        return target_path;
    }
    void symlink::set_target_path(std::string new_target){
        target_path = std::move(new_target);
    }
    fsobject* symlink::resolve(int max_depth) {
       fsobject* root = this;
       while(root->get_parent() != nullptr){
            root = root->get_parent();
       }
       if(root == nullptr || !root->is_directory()){
            return nullptr;
       }
       std::unordered_set<symlink*> visited;
       symlink* current_link = this;
       std::string path_to_resolve = target_path;
       directory* root_dir = static_cast<directory*>(root);
       if(root->is_directory()){
            for(int i =0; i < max_depth; i++){
                if(visited.count(current_link) != 0){
                    return nullptr;
                }
                visited.insert(current_link);
                fsobject* target = root_dir->find_by_path(path_to_resolve);
                if(target == nullptr){
                    return nullptr;
                }
                if(!target->is_symlink()){
                    return target;
                }
                current_link = static_cast<symlink*>(target);
                path_to_resolve = current_link->get_target_path();
            }
       }
        return nullptr;
    }
}