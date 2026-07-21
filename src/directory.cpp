#include "directory.hpp"

namespace filesystem{
    directory::directory(std::string name_) : fsobject(std::move(name_)) {}

    std::size_t directory::get_size() const {
        std::size_t size = 0;
        for(const std::unique_ptr<fsobject>& child : children){
            size += child->get_size();
        }
        return size;
    }
    bool directory::is_directory() const {
        return true;
    }
    std::unique_ptr<fsobject> directory::clone() {
        std::unique_ptr<directory> copy = std::make_unique<directory>(get_name());
        for(std::unique_ptr<fsobject>& child : children){
            copy->add_child(child->clone());
        }
        return copy;
    }
    std::vector<fsobject*> directory::find_all(){
        std::vector<fsobject*> result;
        for(std::unique_ptr<fsobject>& child : children){
            result.push_back(child.get());
            if(child->is_directory()){
                directory* dir = static_cast<directory*>(child.get());
                std::vector<fsobject*> temp = dir->find_all();
                result.insert(result.end(), temp.begin(), temp.end());
            }
        }
        return result;
    }
    std::vector<fsobject*> directory::find_all_current_level(){
        std::vector<fsobject*> result;
        for(std::unique_ptr<fsobject>& child : children){
            result.push_back(child.get());
        }
        return result;
    }
    fsobject* directory::find(const std::string& input_name){
        for(const std::unique_ptr<fsobject>& child : children){
            if(child->get_name() == input_name){
                return child.get();
            }
            
            if(child->is_directory()){
                directory* dir = static_cast<directory*>(child.get());
                fsobject* result = dir->find(input_name);
                if(result){
                    return result;
                }
            }
        }
        return nullptr;
    }
    bool directory::has_child(const std::string& child_name){
        for(std::unique_ptr<fsobject>& child : children){
            if(child->get_name() == child_name){
                return true;
            }
            
            if(child->is_directory()){
                directory* dir = static_cast<directory*>(child.get());
                if(dir->has_child(child_name)){
                    return true;
                }
            }
        }
        return false;
    }
    fsobject* directory::add_child(std::unique_ptr<fsobject> object){
        if(object && validate_name(object->get_name())){
            children.push_back(std::move(object));
            children.back()->set_parent(this);
            return children.back().get();
        }
        return nullptr;
    }
    std::unique_ptr<fsobject> directory::remove_child(const std::string& child_name){
        for(std::size_t i = 0; i < children.size(); i++){
            if(children[i]->get_name() == child_name){
                std::unique_ptr<fsobject> removed = std::move(children[i]);
                for(std::size_t j = i; j < children.size()-1; j++){
                    children[j] = std::move(children[j+1]);
                }
                children.pop_back();
                removed->set_parent(nullptr);
                return removed;
                
            }
        }
        return nullptr;
    }
    bool directory::is_ancestor(fsobject& object){
        for(std::unique_ptr<fsobject>& child : children){
            if(child.get() == &object){
                return true;
            }
            if(child->is_directory()){
                directory* dir = static_cast<directory*>(child.get());
                if(dir->is_ancestor(object)){
                    return true;
                }
            }
        }
        return false;
    }
    fsobject* directory::move(const std::string& name, directory& target){
        fsobject* object = find(name);
        if(!object) return nullptr;
        directory* parent = static_cast<directory*>(object->get_parent());
        if(!parent) return nullptr;
        if(object->is_directory()){
            directory* dir = static_cast<directory*>(object);
            if(dir == &target || dir->is_ancestor(target)) return nullptr;
        }
        if(target.validate_name(name)){
            std::unique_ptr<fsobject> removed = parent->remove_child(name);
            return target.add_child(std::move(removed));
        }
        return nullptr;
    }
    bool directory::validate_name(const std::string& new_name){
        if(is_directory()){
            std::vector<fsobject*> current_level = find_all_current_level();
            for(fsobject* child : current_level){
                if(child->get_name() == new_name){
                    return false;
                }
            }
        }
        return true;
    }
        
}
