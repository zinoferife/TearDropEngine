#pragma once
#include <type_traits>
#include <cstring>

#include <string>
#include <tuple>


namespace td
{
	//type name from type 
	//might not work for all compilers
	namespace helper {
		static constexpr std::uint32_t FRONT_SIZE = sizeof("td::helper::get_type_name_helper<") - 1u;
		static constexpr std::uint32_t BACK_SIZE = sizeof(">::get_type_name") - 1u;

		template<typename T>
		struct get_type_name_helper
		{
            //hack not working clang
			static const char* get_type_name() {
				static constexpr size_t size = sizeof(__FUNCTION__) - FRONT_SIZE - BACK_SIZE;
				static char type_name[10] = {};
				//memcpy(type_name, __FUNCTION__ + FRONT_SIZE, size - 1u);
				return type_name;
			}
		};
	}

	template<typename T>
	inline const char* get_type_name()
	{
		return td::helper::get_type_name_helper<T>::get_type_name();
	}

	namespace detail
	{
		template<typename T, typename U> struct index;

		template<typename U, typename T>
		struct index<std::tuple<U>, T>
		{
			enum { value = -1 };
		};

		template<typename T>
		struct index<std::tuple<T>, T>
		{
			enum { value = 0 };
		};


		template<typename T, typename...U>
		struct index<std::tuple<T, U...>, T>
		{
			enum { value = 0 };
		};

		template<typename T, typename U, typename... S>
		struct index<std::tuple<U, S...>, T>
		{
		private:
			enum { temp = index<std::tuple<S...>, T>::value };
		public:
			enum { value = temp == -1 ? -1 : 1 + temp };
		};
	};
    
    template<size_t i, size_t u>
    struct integral_equal : public std::integral_constant<bool, (i == u)> {};
    

};

