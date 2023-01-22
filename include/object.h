#pragma once

#include <string>
#include <string_view>

#include <memory>
#include <stdint.h>

#include <vector>
#include <iterator>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <chrono>


#include "type_traits.h"

namespace td {
    class stream;
    class link;
    class controller;
	class object : std::enable_shared_from_this<object>
	{
	public:
		typedef std::uint64_t id_type;
		typedef std::string name_type;
        typedef std::function<object* (td::stream&) > factory_func_t;
        typedef std::unordered_map<const char*, factory_func_t> factory_map_t;

		object() = default;
		object(const std::string& name, std::uint64_t id);
		virtual ~object();


		inline id_type get_id() const { return m_id; }
		inline const std::string& get_name() const { return m_name; }
        
        //derived classes would implement this with the type of class 
        virtual std::string_view get_type_name() const {
                return std::string_view{td::get_type_name<object>()};
        } 
        
        
        inline std::shared_ptr<const object> share() const {
                return shared_from_this();
        }

        inline std::weak_ptr<const object> share_weak() const {
            return shared_from_this();
        }
        
        
        inline std::shared_ptr<object> share() {
                return shared_from_this();
        }
        
        //for now, think of a way to remove the dynamic cast
        template<typename T, std::enable_if_t<std::is_base_of_v<object, T>,int> = 0>
        inline auto share() -> std::shared_ptr<T>{
            return std::dynamic_pointer_cast<T>(shared_from_this());
        }

        //unique pointers cannot be copied, only moved
        inline void set_controller(controller* control){
            //most be created on the heap, owned by the object
            controllers.emplace_back(control);
        }
        
        template<typename T, typename... Args>
        void emplace_controller(Args... args)
        {
                static_assert(std::is_base_of_v<controller, T>, "T is not a controller type");
                controllers.emplace_back( new T(args...));
        }
        
        template<typename T>
        void emplace_controller()
        {
                static_assert(std::is_base_of_v<controller, T>, "T is not a controller type");
                controllers.emplace_back(new T);
        }
        
        inline size_t get_controller_count() const { return controllers.size(); }
        inline const controller* get_controller(size_t i) const {
                return controllers.at(i);
        }
        
        bool update_controllers(double app_time);
        
        //because of dependency
        
        inline void remove_all_controllers();
        bool remove_controller_at(size_t i);
        bool remove_controller_by_name(const std::string& name);
        bool remove_controller_by_id(id_type id);
        
        
        //streaming operations
    public:
        static factory_map_t factory_map;
        template<class T>
        static bool register_factory()
        {
                static_assert(std::is_base_of_v<object, T>, "T is not derived from object or a type of object");
                const char* class_name = td::get_type_name<T>();
                auto pair = factory_map.insert({class_name, T::factory});
                return pair.second;
        }
        
        
        static void init_factory();
        static void terminate_factory();
        
        static object* factory(td::stream& stream);
        
        virtual void load(td::stream& stream, link* link);
        virtual void link(td::stream& stream, link* link);
        
        virtual bool register_obj(td::stream& stream) const;
        virtual bool save(td::stream& stream) const;
        
        virtual size_t get_memory_used() const;
        virtual size_t get_disk_used() const;
        
	protected:
		//object identification
		id_type m_id;
		name_type m_name;        
        
        
        
    private:
        //controllers are not really shared, so hold unique pointers to them. 
        std::vector<controller*> controllers;
        
        
	};
    
    typedef std::shared_ptr<object> object_shared_ptr;
    typedef std::weak_ptr<object> object_weak_ptr;
    
    template<typename T>
    auto object_cast(object_shared_ptr ptr) -> std::shared_ptr<T>
    {
            static_assert(std::is_base_of_v<T, object>, "T is not derived from object");
            return std::dynamic_pointer_cast<T>(ptr);
    }
}
