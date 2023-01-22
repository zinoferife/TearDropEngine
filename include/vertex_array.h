#pragma once
#include <glm/glm.hpp>
#include <vector>

#include <type_traits>
#include <tuple>
#include <array>

#include "type_traits.h"


//a collection of verties that is positions
namespace td{
	template<glm::length_t d, typename T,  glm::qualifier q = glm::qualifier::highp>
	using vertex_array  = std::vector<glm::vec<d, T, q>>;

	typedef vertex_array<2, float> vertex_array2f;
	typedef vertex_array<2, double> vertex_array2d;
	typedef vertex_array<3, float> vertex_array3f;
	typedef vertex_array<3, double> vertex_array3d;



	//vetex formats
	//positions
	//normals 
	//texture codinates
	template<typename T>
	struct vertex_VNT
	{
		union {
			struct {
				T px;
				T py;
				T pz;
				T nx;
				T ny;
				T nz;
				T tx;
				T ty;
			};
			T data[8];
		};
	};

	template<typename T>
	using vertex_array_VNT = std::vector<vertex_VNT<T>>;
	typedef vertex_array_VNT<float> vertex_array_VNTf;
	typedef vertex_array_VNT<double> vertex_array_VNTd;


	//positions, normals, colors, texture coords, etc...
	template<typename T>
	struct vertex_VNCT
	{
		union {
			struct {
				T px;
				T py;
				T pz;
				T nx;
				T ny;
				T nz;
				T r;
				T g;
				T b;
				T s;
				T t;
			};
			T data[11];
		};

	};
	template<typename T>
	using vertex_array_VNCT = std::vector<vertex_VNCT<T>>;
	typedef vertex_array_VNCT<float> vertex_array_VNCTf;
	typedef vertex_array_VNCT<double> vertex_array_VNCTd;


	//position
	//normal
	//joint id,
	//joint weight
	// texture coords
	template<typename T, typename I = std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>>
	struct vertex_VNJWT
	{
		typedef T value_type;
		typedef I index_type;
		struct {
			T px;
			T py;
			T pz;

			T nx;
			T ny;
			T nz;

			I jx;
			I jy;
			I jz;

			T wx;
			T wy;
			T wz;

			T tx;
			T ty;
		};
	};



	//format type traits for opengl
	enum tbtype
	{
		td_float,
		td_double,
		td_char,
        td_uchar,
        td_int32,
 		td_uint32,
		td_null
	};

	namespace detail
	{
		
		template<class T>
		struct has_specifications : std::false_type {};


		template<glm::length_t d, typename T, glm::qualifier q>
		struct has_specifications<glm::vec<d, T, q>>
		 : public std::true_type {};

		
		template<typename T>
		struct has_specifications<vertex_VNT<T>>
		: public std::true_type {};

		template<typename T>
		struct has_specifications<vertex_VNCT<T>>
			: public std::true_type {};

		template<typename T>
		struct has_specifications<vertex_VNJWT<T>>
			: public std::true_type {};

		using tb_c_types = std::tuple<float, double, char, std::uint8_t ,std::int32_t, std::uint32_t>;
		
		template<typename T>
		struct type_to_tbtype
		{
            using value_type = std::decay_t<T>;
			enum { value = index<tb_c_types, value_type>::value };
			static constexpr tbtype type = value == -1 ? tbtype::td_null : tbtype(value);
		};

		template<typename T>
		struct is_vertex_format
		: std::integral_constant<bool , std::conjunction_v<std::is_standard_layout<T>, has_specifications<T>>>{
			//if has specification and is standard layout it is a correct vertex format. 
			// 
		};
	}
	//format specifications
	//format specifications are tuples that contain, 
	

	//type,
	//size the componet count,
	enum specs : std::uint8_t
	{
		sp_type = 0,
		sp_component_count,
		sp_relative_offet
	};

	template<class vert_format_type>
	struct specification;

	using specs_type = std::tuple<tbtype, size_t, size_t>;
	template<glm::length_t d, typename T, glm::qualifier q>
	struct specification<glm::vec<d, T, q>>
	{
		typedef std::array<specs_type, 1> spec_array_type;
		static spec_array_type spec_values;

		const spec_array_type& operator()() const
		{
			return spec_values;
		}
	};
	template<glm::length_t d, typename T, glm::qualifier q>
	std::array<specs_type, 1> specification<glm::vec<d, T, q>>::spec_values = {
		{detail::type_to_tbtype<T>::type, d, 0}
	};


	template<typename T>
	struct specification<vertex_VNT<T>>
	{
		typedef std::array<specs_type, 3> spec_array_type;
		static spec_array_type spec_values;

		const spec_array_type& operator()() const
		{
			return spec_values;
		}
	};

	template<typename T>
	std::array<specs_type, 3> specification<vertex_VNT<T>>::spec_values = {
		specs_type{detail::type_to_tbtype<T>::type, 3ul, static_cast<unsigned long>(offsetof(vertex_VNT<T>, px))},
		specs_type{detail::type_to_tbtype<T>::type, 3ul, static_cast<unsigned long>(offsetof(vertex_VNT<T>, nx))},
		specs_type{detail::type_to_tbtype<T>::type, 2ul, static_cast<unsigned long>(offsetof(vertex_VNT<T>, tx))}
	};


	template<typename T>
	struct specification<vertex_VNCT<T>>
	{
		typedef std::array<specs_type, 4> spec_array_type;
		static spec_array_type spec_values;

		const spec_array_type& operator()() const
		{
			return spec_values;
		}
	};

	template<typename T>
	std::array<specs_type, 4> specification<vertex_VNCT<T>>::spec_values = {
		specs_type{detail::type_to_tbtype<T>::type, 3, 0},
		specs_type{detail::type_to_tbtype<T>::type, 3, sizeof(T) * 3},
		specs_type{detail::type_to_tbtype<T>::type, 3, 2 * (sizeof(T) * 3)},
		specs_type{detail::type_to_tbtype<T>::type, 2, 3 * (sizeof(T) * 3)}
	};

	template<typename T>
	struct specification<vertex_VNJWT<T>>
	{
		typedef std::array<specs_type, 5> spec_array_type;
		static spec_array_type spec_values;

		const spec_array_type& operator()() const
		{
			return spec_values;
		}
	};

	template<typename T>
	std::array<specs_type, 5> specification<vertex_VNJWT<T>>::spec_values = {
		specs_type{detail::type_to_tbtype<T>::type, 3, 0},
		specs_type{detail::type_to_tbtype<T>::type, 3, sizeof(T) * 3},
		specs_type{detail::type_to_tbtype<typename vertex_VNJWT<T>::index_type>::type, 3, 2 * (sizeof(typename vertex_VNJWT<T>::index_type) * 3)},
		specs_type{detail::type_to_tbtype<T>::type, 3, 3 * (sizeof(T) * 3)},
		specs_type{detail::type_to_tbtype<T>::type, 2, 4 * (sizeof(T) * 3)},
	};

}

