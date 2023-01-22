#pragma once
#include <type_traits>
#include "type_traits.h"


#include <iostream>
#include <vector>
#include <functional>


//simple refection system, to help with streaming of data from and to files 
//removes the need to have a single inheritance class hierachy to provide different 


namespace td
{
	namespace reflect {

		template<typename T> static char func(decltype(&T::reflection));
		template<typename T> static int func(...);

		template<typename T>
		struct is_reflected
		{
			enum {
				value = (sizeof(func<T>(nullptr)) == sizeof(char))
			};
		};


		struct base_type_descriptor
		{
			virtual ~base_type_descriptor() {}
		};


		template<typename T>
		struct type_descriptor : public base_type_descriptor
		{
			using desc_type = T;
			const char* name = td::get_type_name<T>();
			static constexpr size_t size = sizeof(T);
			struct member
			{
				const char* name;
				size_t offset;
				base_type_descriptor* type_desc;
			};

			std::vector<member*> m_members;

			constexpr const char* get_name() const { return name; }
			inline const std::vector<member*>& get_members_array() const {
				return m_members;
			}
			inline std::vector<base_type_descriptor*>& get_members_array() {
				return m_members;
			}
            typedef std::function<void(type_descriptor*)> init_func_t;
			type_descriptor(init_func_t init){
				init(this);
			}

		};

		//dump class, this class is specialised for each type descriptor, 
		template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
		struct dump
		{
			//
			static void put(type_descriptor<T>* type_d, void* datastore)
			{
			
			}

			static void put(type_descriptor<T>* type_d, std::ostream& in_stream){
			
			}
		};


		template<typename T>
		auto get_primitive_descriptor()-> type_descriptor<T>*
		{
			
		}

		struct default_resolver {
			template<typename T, std::enable_if_t<is_reflected<T>::value, int> = 0>
			static auto get() -> type_descriptor<T>*
			{
				//T has a static member declared as reflection in its body
				return &T::reflection;
			}

			template<typename T, std::enable_if_t<std::disjunction_v<std::is_floating_point<T>, std::is_integral<T>>, int> = 0>
			static auto get() -> type_descriptor<T>*
			{
				return get_primitive_descriptor<T>();
			}
		};

		template<typename T>
		struct type_resolver
		{
			static auto get() -> type_descriptor<T>*
			{
				return default_resolver::get<T>();
			}

		};

	}

#define REFLECT(T) \
		friend struct refelct::default_resolver; \
		static reflect::type_descriptor<T> reflection; \
		static void init_reflection(reflect::type_descriptor<T>*); 
#define REFLECT_BEGIN(T) \
		reflect::type_descriptor<T> T::reflection{T::init_reflection}; \
		void T::init_reflection(reflect::type_descriptor<T>* t_des) { \
				using type = T; \
				t_des->members = { 

#define REFLECT_MEMBER(name) \
				{#name, offsetof(type, name), reflect::type_resolver<decltype(type::name)>::get()},

#define REFLECT_END() \
			}; \
		}

}

