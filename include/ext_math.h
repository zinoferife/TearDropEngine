#pragma once
#include <math.h>
#include <type_traits>
#include <glm/glm.hpp>
#include <numeric>


namespace td
{
	namespace math {
		template<typename t, glm::qualifier q>
		void orthonormalise(const glm::vec<2, t, q>& v0,
			const glm::vec<2, t, q>& v1, 
			glm::vec<2, t, q>& u0, glm::vec<2, t, q>& u1)
		{
			u0 = glm::normalize(v0);
			t dot = glm::dot(u0, v1);
			auto temp = v1 - (u0 * dot);
			u1 = glm::normalize(temp);
		}

		template<typename T, glm::qualifier q>
		glm::vec<2, T, q> prep(const glm::vec<2, T, q>& v)
		{
			return glm::vec<2, T, q>(-v[1], v[0]);
		}

		template<typename T, glm::qualifier q>
		void generate_orthonormal_basis(const glm::vec<2, T, q>& u0,
			glm::vec<2, T, q>& u, glm::vec<2, T, q>& v, bool unit_length = false)
		{
			if (!unit_length) {
				u = glm::normalize(u0);
			}
			else {
				u = u0;
			}
			v = prep(u0);
		}

		template<typename T, glm::qualifier q>
		void orthonormalise(glm::vec<3, T, q>& u0,
			glm::vec<3, T, q>& u1, glm::vec<3, T, q>& u2)
		{
			u0 = glm::normalize(u0);
			
			T dot = glm::dot(u0, u1);
			auto temp = u1 - (u0 * dot);
			u1 = glm::normalize(temp);

			T d02 = glm::dot(u0, u2);
			T d12 = glm::dot(u1, u2);

			temp = u2 - (u0 * d02) - (u1 * d12);
			u2 = glm::normalize(temp);
		}

		template<typename T, glm::qualifier q>
		void generate_orthonormal_basis(const glm::vec<3, T, q>& w,
			glm::vec<3, T, q>& u0, glm::vec<3, T, q>& u1, glm::vec<3, T, q>& u2)
		{
			u0 = glm::normalize(w);
			if (glm::abs(w[0]) >= glm::abs(w[1]))
			{
				u1[0] = -w[2];
				u1[1] = 0;
				u1[2] = +w[0];
			}
			else {
				u1[0] = 0;
				u1[1] = +w[2];
				u1[2] = -w[1];
			}
			u1 = glm::normalize(u1);
			u2 = glm::cross(u0, u1);
		}

		template<glm::length_t c, glm::length_t r, typename T, glm::qualifier q>
		glm::mat<c, r, T, q> transpose_times(const glm::mat<c, r, T, q>& m0,
			const glm::mat<c, r, T, q>& m1)
		{
			typename glm::mat<c, r, T, q>::transpose_type temp_mat = glm::transpose(m0);
			return (temp_mat * m1);
		}


		template<glm::length_t c, glm::length_t r, typename T, glm::qualifier q>
		glm::mat<c, r, T, q> times_transpose(const glm::mat<c, r, T, q>& m0,
			const glm::mat<c, r, T, q>& m1)
		{
			typename glm::mat<c, r, T, q>::transpose_type temp_mat = glm::transpose(m1);
			return (m0 * temp_mat);
		}

		//for 2d matries
		template<typename T, glm::qualifier q>
		void eigan_deomposition(const glm::mat<2, 2, T, q>& vmat, glm::mat<2, 2, T, q>& e_vectors, glm::mat<2, 2, T, q>& e_values)
		{


		}

		//for 3d matrices
		template<typename T, glm::qualifier q>
		void eigan_deomposition(const glm::mat<3, 3, T, q>& vmat, glm::mat<3, 3, T, q>& e_vectors, glm::mat<3, 3, T, q>& e_values)
		{
			
				
		}
		
		template<typename T, glm::qualifier q>
		glm::mat<3, 3, T, q> from_axis_angle(const glm::vec<3, T, q>& axis, T angle)
		{
			static_assert(std::is_floating_point<T>::value, "Type must be a floating point type to work properly");
			typedef glm::mat<3, 3, T, q> mat3;
			typedef typename mat3::column_type col_type;
			mat3 S, I(1.0), R, S2;
			S[0] = col_type(0, axis[2], -axis[1]);
			S[1] = col_type(-axis[2], 0, axis[0]);
			S[3] = col_type(axis[1], -axis[0], 0);
			S2 = S * S;
			S2 = S2 * (1 - glm::cos(angle));
			S = S * glm::sin(angle);

			R = I + S + S2;
			return R;
		}

		template<typename T, glm::qualifier m, glm::qualifier q>
		glm::mat<3, 3, T, q> qut_to_rot_matrix(const glm::qua<T, m>& qua)
		{
			typedef glm::mat<3, 3, T, q> mat3;
			mat3 R;
			typedef typename mat3::col_type col_type;
			R[0] = col_type((1 - (2 * qua.y * qua.y) - (2 * qua.z * qua.z)),
						   ((2 * qua.x * qua.y) - (2 * qua.w * qua.z)),
						   ((2 * qua.x * qua.z) + (2 * qua.w * qua.y)));

			R[1] = col_type(((2 * qua.x * qua.y) + (2 * qua.w * qua.z)), 
							(1 - (2 * qua.x * qua.x ) - (2 * qua.z * qua.z)),
							((2 * qua.y * qua.z) - (2 * qua.w * qua.x)));

			R[2] = col_type(((2 * qua.x * qua.z) - (2 * qua.w * qua.y)), 
							((2 * qua.y * qua.z) + (2 * qua.w * qua.x)),
							(1 - (2 * qua.x * qua.x) - (2 * qua.y * qua.y)));
			return R;
		}


		//a quternion that rotates v1 to v2
		//test if v1 and v2 are parrallel and return a zero quat?? 
		template<typename T, glm::qualifier q>
		glm::qua<T, q> align(const glm::vec<3, T, q>& v1, const glm::vec<3, T, q>& v2)
		{
			auto bi_vector = glm::normalize((v1 + v2));
			T cos02 = glm::dot(v1, bi_vector);
			auto c = glm::cross(v2, bi_vector);
			c = c + cos02;

			return glm::qua<T, q>(0, c.x, c.y, c.z);
		}

		template<typename T>
		T saturate(T value) {
			static_assert(std::is_floating_point_v<T>, "T must be a floating point type");
			return glm::min(glm::max(value, static_cast<T>(0.0)), static_cast<T>(1.0));
		}

		template<glm::length_t d, typename T, glm::qualifier q>
		T angle(const glm::vec<d, T, q>& vec1, const glm::vec<d, T, q>& vec2)
		{
			auto norm_1 = glm::normalize(vec1);
			auto norm_2 = glm::normalize(vec2);
			T dotp = glm::dot(norm_1, norm_2);
			return glm::acos(dotp);
		}

        template<typename T, glm::qualifier q = glm::defaultp>
        T signed_2d_tri_area(const glm::vec<2, T, q>& a, const glm::vec<2, T, q>& b, 
            const glm::vec<2, T, q>& c) {
                return ((a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x * c.x));    
        }
        
        
	}
}