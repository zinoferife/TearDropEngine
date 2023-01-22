#include "object.h"
#include "controller.h"
#include "stream.h"

//mem for the factory map
td::object::factory_map_t td::object::factory_map{};


td::object::object(const std::string& name, std::uint64_t id)
: m_name(name), m_id(id) {
    //controllers are empty
}
bool td::object::remove_controller_at(size_t i)
{
    auto iter = std::next(controllers.begin(), i);
    if(iter != controllers.end()){
        controllers.erase(iter);
        return true;
    }
    return false;
}

td::object::~object() {
        if(!controllers.empty()){
            remove_all_controllers();
        }
}

bool td::object::remove_controller_by_name(const std::string& name)
{
    auto found = std::find_if(controllers.begin(), controllers.end(), [&](auto& elem){
            return (elem->get_name() == name);
    });
    if(found != controllers.end()) {
            if((*found)) delete (*found);
            controllers.erase(found);
            return true;
    }else {
            return false;
    }
}
bool td::object::remove_controller_by_id(td::object::id_type id)
{
    auto found = std::find_if(controllers.begin(), controllers.end(), [&](auto& elem){
            return (elem->get_id() == id);
    });
    if(found != controllers.end()) {
            if((*found)) delete (*found);
            controllers.erase(found);
            return true;
    }else {
            return false;
    }
}

void td::object::init_factory()
{
    
}
void td::object::terminate_factory()
{
    
}
void td::object::load(td::stream& stream, td::link* link)
{
    
}
void td::object::link(td::stream& stream, td::link* link)
{
    
}
bool td::object::register_obj(td::stream& stream) const
{
    object* pkThis = const_cast<object*>(this);
    //the rest for these functions i have no idea how it works lol
    
    return false;
}
bool td::object::save(td::stream& stream) const
{
    std::string_view view = get_type_name();
    stream << view.size();
    stream.write(reinterpret_cast< const char*>(view.data()), view.size());
    
    //write the pointer for identification 
    stream << this;
    
    //write object name 
    stream << m_name.size();
    stream.write(reinterpret_cast< const char*>(m_name.data()), m_name.size());
    
    stream << controllers.size();
    for(auto iter = controllers.begin(); iter != controllers.end(); iter++)
    {
            //write the address of each controller pointer
            stream << *iter;
    }
    //something went wrong on the stream object
    if(stream.bad()) return false;
    return true;
}

size_t td::object::get_memory_used() const
{
    size_t size = 0;

    return size;
}
size_t td::object::get_disk_used() const
{
    size_t size = 0;

    return size;
}

void td::object::remove_all_controllers()
{
    for(auto& ptr : controllers){
        if(ptr) delete ptr;
    }
    
}
bool td::object::update_controllers(double app_time)
{
      bool updated = false;
    for(auto iter = controllers.begin(); iter != controllers.end();iter++){
        if((*iter)){
                    //if the pointer exisit, do the update,
                    //cyclic dependencies is causing a problem
                    updated |= (*iter)->update(app_time);
                    }else{
                        //remove it from the list
                        controllers.erase(iter);
                    }
        }
        return updated;
}
