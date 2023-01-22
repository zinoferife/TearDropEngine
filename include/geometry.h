#pragma once
#include "ext_math.h"
#include <assert.h>


namespace td {
	namespace geo
	{
		//all the gemometric_shapes that can be defined mathematically would be defined here,
		//these include lines, rays, cirles, spheres, planes
		
		typedef glm::vec<2, float, glm::qualifier::highp> point2f;
		typedef glm::vec<2, double, glm::qualifier::highp> point2d;
		typedef glm::vec<3, float, glm::qualifier::highp> point3f;
		typedef glm::vec<3, double, glm::qualifier::highp> point3d;

		template<glm::length_t d, typename T,  glm::qualifier q>
		struct line
		{
			typedef T value_type;
			typedef glm::vec<d, T, q> vec_type;
			line() = default;
			explicit line(const vec_type& p1, const vec_type& p2)
			{
				data[0] = p1;
				data[1] = p2;
			}
			union {
				struct {
					vec_type p0;
					vec_type p1;
				};
				vec_type data[2];
			};
			const vec_type& operator[](size_t i) const{
				//assert repair
#ifdef	DEBUG
				assert(i >= 0 && i < d && "index is out of range");
#endif // DEBUG
				return data[i];
			}

			vec_type& operator[](size_t i)
			{
				//assert repair
#ifdef DEBUG
				assert(i >= 0 && i < d && "index is out of range");
#endif //DEBUG
				return data[i];

			}

		};
		typedef line<2, float, glm::qualifier::highp> line2f;
		typedef line<2, double, glm::qualifier::highp> line2d;
		typedef line<3, float, glm::qualifier::highp> line3f;
		typedef line<3, double, glm::qualifier::highp> line3d;

		template<glm::length_t d, typename T, glm::qualifier q>
		struct ray
		{
			typedef T value_type;
			typedef glm::vec<d, T, q> vec_type;

			ray() = default;
			explicit ray(const vec_type& dir, const vec_type& start):
				direction(dir), origin(start)
			{
			}
			union
			{
				struct{
					vec_type direction;
					vec_type origin;
				};
				vec_type data[2];
			};
			const vec_type& operator[](size_t i) const
			{
#ifdef DEBUG
				assert(i >= 0 && i < d && "index is out range");
#endif // DEBUG
				return data[i];
			}

			vec_type& operator[](size_t i)
			{
#ifdef DEBUG
				assert(i >= 0 && i < d && "index is out range");
#endif //DEBUG
				return data[i];
			}

		};

		typedef ray<2, float, glm::qualifier::highp> ray2f;
		typedef ray<2, double, glm::qualifier::highp> ray2d;
		typedef ray<3, float, glm::qualifier::highp> ray3f;
		typedef ray<3, double, glm::qualifier::highp> ray3d;

		template<typename T, glm::qualifier q>
		struct sphere
		{
			typedef T value_type;
			sphere() = default;
			explicit sphere(const glm::vec<3, T, q>& cen, T rad)
				: center(cen), radius(rad)
			{
			
			}
			glm::vec<3, T, q> center;
			T radius;
		};

		typedef sphere<float, glm::qualifier::highp> spheref;
		typedef sphere<double, glm::qualifier::highp> sphered;

		template<typename T, glm::qualifier q = glm::qualifier::highp>
		struct plane
		{
			typedef T value_type;
			plane() = default;
			explicit plane(const glm::vec<3, T, q>& norm, T con)
				: normal(norm), constant(con)
			{
			
			}

			explicit plane(const glm::vec<3, T, q>& norm, const glm::vec<3, T, q>& point)
			{
				normal = norm;
				constant = glm::dot(normal, point);
			}

			explicit plane(const glm::vec<3, T, q>& p0, const glm::vec<3, T, q>& p1, const glm::vec<3, T, q>& p2)
			{
				auto line1 = p1 - p0;
				auto line2 = p2 - p0;
				auto n = glm::cross(line2, line1);
				normal = glm::normalize(n);
				constant = glm::dot(normal, p0);
			}

			static int which_side(const plane& pl, const glm::vec<3, T, q>& point)
			{
				static_assert(std::is_floating_point_v<T>, "vector must be a floating point type as value type");
				auto distance = glm::dot(pl.normal, point) - pl.constant;
				if (distance > static_cast<T>(0.0)) {
					return 1;
				}
				else if (distance < static_cast<T>(0.0)) {
					return -1;
				}
				else {
					return 0;
				}
			}

			glm::vec<3, T, q> normal;
			T constant;

		};

		typedef plane<float> plane3f;
		typedef plane<double> planedf;

		//returns a matrix that projects points to the plane, in the direction specified by projection direction
		template<typename T, glm::qualifier q>
		glm::mat<4, 4, T, q> make_oblique_projection(const plane<T, q>& plane, const glm::vec<3, T, q>& projection_direction)
		{
			typedef glm::mat<4, 4, T, q> mat4;
			typedef typename mat4::column_type vec_type;

			T d_n = glm::dot(plane.normal, projection_direction);
			mat4 identity {1.0};

			if (d_n == static_cast<T>(0)) {
				//projectin is parallel 
				return identity;
			}
			vec_type c0(projection_direction.x * plane.normal.x, projection_direction.x * plane.normal.y, projection_direction.x * plane.normal.z);
			vec_type c1(projection_direction.y * plane.normal.x, projection_direction.y * plane.normal.y, projection_direction.y * plane.normal.z);
			vec_type c2(projection_direction.z * plane.normal.x, projection_direction.z * plane.normal.y, projection_direction.x * plane.normal.z);

			glm::mat<3, 3, T, q> mat(c0, c1, c2);

		}


		template<typename T, glm::qualifier q  = glm::qualifier::highp>
		struct capsule
		{
			static_assert(std::is_floating_point_v<T>, "T must be a floating point type");
			typedef glm::vec<3, T, q> vec_type;
			typedef T value_type;


			vec_type base;
			vec_type tip;
			T radius;
			capsule() = default;
			explicit capsule(const vec_type& b, const vec_type& t, T r)
				: base(b), tip(t), radius(r)
			{
				
			}

		};

		template<typename T, glm::qualifier q>
		struct rect
		{
			typedef glm::vec<2, T, q> vec_type;
			typedef T value_type;
			rect() = default;
			explicit rect(const vec_type& o, const vec_type& s)
			: position(o), size(s){
			
			}

			explicit rect(T x, T y, T w, T h)
				: position(x, y), size(w, h)
			{}

			vec_type position;
			vec_type size;

		};

        //axis aligned and oriented bounding boxes
        template<glm::length_t d, typename T, glm::qualifier q>
        struct aabb
        {
            static_assert(d > 1 && d <= 3, "Cannot have dimensions above 3 or less than 2");
            typedef glm::vec<d, T, q> vec_type;
            vec_type min;
            vec_type max;
            
            explicit aabb(const vec_type& m, const vec_type& mx)
            : min(m), max(mx){}
        };
        
        
        template<glm::length_t d, typename T, glm::qualifier q>
        struct obb
        {
                static_assert(d > 1 && d <= 3, "Cannot have dimensions above 3 or less than 2");
                typedef glm::vec<d, T, q> vec_type;
                vec_type axis[d];
                vec_type center;
                T exts[d];
                
                //different construction
        };
        
        template<glm::length_t d, typename T, glm::qualifier q>
        struct lozenge
        {
                typedef glm::vec<d, T, q> vec_type;
                vec_type point;
                vec_type axis[2];
                T radius;
        };
        
        
        //infinate space between two parallel planes, given by the signed distance; n_far and n_near;
        template<glm::length_t d, typename T, glm::qualifier q>
        struct slab
        {
                typedef glm::vec<d, T, q> vec_type;
                vec_type normal;
                T n_far;
                T n_near;
        };
        
        template<typename T, size_t K>
        struct kdop
        {
                typedef T real_type;
                inline constexpr const size_t k() const {return K;}
                constexpr static size_t arr_s = static_cast<size_t>(K/2);
                T min[arr_s];
                T max[arr_s];
        };
        
        //holds 3 indices into vertex buffers to reprents triangle
        struct triangle{
            glm::uvec3 videx;
        };
        
        //hold 2 indices into vertex buffers to represent edges 
        struct edge {
                glm::uvec2 vidx;
        };
        
        
	}
}