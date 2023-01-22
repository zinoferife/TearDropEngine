#pragma once
#include "geometry.h"
#include "distance.h"
#include "closest_points.h"

namespace td
{
	//intersection data type
	template<typename T, glm::length_t d, glm::qualifier q = glm::qualifier::highp>
	using intersection_data_t = std::tuple<glm::vec<d, T, q>, glm::vec<d, T, q>, T>;
	enum {
		intersect_normal,
		intersect_point,
		intersect_penetration_depth
	};

	typedef intersection_data_t<float, 2> intersection_data_t2f;
	typedef intersection_data_t<double, 2> intersection_data_t2d;
	typedef intersection_data_t<float, 3> intersection_data_t3f;
	typedef intersection_data_t<double, 3> intersection_data_t3d;


	//intersection tests
	template<typename T, glm::qualifier q>
	T ray_sphere_intesection(const geo::ray<3, T, q>& r, const geo::sphere<T, q>& sp)
	{
		T radius2 = sp.radius * sp.radius;
		T d_d = glm::dot(r.direction, r.direction);
		auto k = r.origin - sp.center;

		auto a = d_d;
		auto b = glm::dot(r.direction, k) * 2;
		auto c = glm::dot(k, k) - radius2;

		//solve the equation
	}

	template<typename T, glm::qualifier q>
	bool sphere_sphere_intersection(const geo::sphere<T, q>& sphere1, 
		const geo::sphere<T, q>& sphere2,
		intersection_data_t<T, 3 ,q>& intersection_data)
	{
		auto distance = glm::distance(sphere1.center, sphere2.center);
		auto radii = sphere1.radius + sphere2.radius;
		if (distance < radii){
			//we are intersecting
            
            
            return true;
		}

		return false;
	}

	//assumes that the 3 points froms a triangle and are co-planer
	template<typename T, glm::qualifier q>
	bool sphere_triangle_intersection(const geo::sphere<T, q>& sphere, const glm::vec<3, T, q>& p0,
		const glm::vec<3, T, q>& p1,  const glm::vec<3, T, q>& p2, intersection_data_t<T, 3, q>& intersection_data)
	{
		glm::vec<3, T, q> tri_norm = glm::normalize(glm::cross((p1 - p0), (p2 - p0)));
		T dist = glm::dot(sphere.center - p0, tri_norm); // signed distance
		if (dist < -sphere.radius && dist > sphere.radius){
			//no intersection
			return false;
		}
		auto proj_point = sphere.center - (tri_norm * dist);
		auto c0 = cross(proj_point - p0, p1 - p0);
		auto c1 = cross(proj_point - p1, p2 - p1);
		auto c2 = cross(proj_point - p2, p0 - p2);
		//if inside, the closest point is the projected center point

		bool inside = glm::dot(c0, tri_norm) <= 0 &&
			glm::dot(c1, tri_norm) <= 0 &&
			glm::dot(c2, tri_norm) <= 0;
		//find closest point on one of the edges on the triangle to the sphere center
		T radii = sphere.radius * sphere.radius;

		//edge 1:
		auto point1 = closest_point_on_line(sphere.center, geo::line<3, T, q>{ p0, p1 });
		auto v1 = sphere.center - point1;
		T distsql = glm::dot(v1, v1);
		bool intersects = distsql < radii;

		//edge2:
		auto point2 = closest_point_on_line(sphere.center, geo::line<3, T, q>{ p1, p2 });
		auto v2 = sphere.center - point2;
		T distsql2 = glm::dot(v2, v2);
		intersects |= distsql2 < radii;

		//egde3:
		auto point3 = closest_point_on_line(sphere.center, geo::line<3, T, q>{ p2, p0 });
		auto v3 = sphere.center - point3;
		T distsql3 = glm::dot(v3, v3);
		intersects |= distsql3 < radii;

		if (inside || intersects) {
			auto best_point = proj_point;
			glm::vec<3, T, q> intersection_vec;
			if (inside) {
				intersection_vec = sphere.center - proj_point;
			}
			else {
				auto d = sphere.center - point1;
				T best_distsq = glm::dot(d, d);
				best_point = point1;
				intersection_vec = d;

				d = sphere.center - point2;
				T distsq = glm::dot(d, d);
				if (distsq < best_distsq) {
					distsq = best_distsq;
					best_point = point2;
					intersection_vec = d;
				}

				d = sphere.center - point3;
				distsq = glm::dot(d, d);
				if (distsq < best_distsq) {
					distsq = best_distsq;
					best_point = point3;
					intersection_vec = d;
				}
			}

			auto len = glm::length(intersection_vec);
			std::get<intersect_normal>(intersection_data) = intersection_vec / len;
			std::get<intersect_penetration_depth>(intersection_data) = sphere.radius - len; 
			std::get<intersect_point>(intersection_data) = best_point;
			return true;
		}

		return false;
	}


	/// <summary>
	/// implementation based on wickedengine by 
	/// link: 
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="capsule1"></param>
	/// <param name="capsule2"></param>
	/// <param name="intersection_data"></param>
	/// <returns>bool: true if there is an intersection false if no intersection</returns>
	template<typename T, glm::qualifier q>
	bool capsule_capsule_intersection(const geo::capsule<T, q>& capsule1,
		const geo::capsule<T, q>& capsule2, td::intersection_data_t<T, 3, q>& intersection_data)
	{
		typedef typename td::geo::capsule<T, q>::vec_type vec_type;
		typedef T real_type;

		//capsule 1,
		vec_type normA = glm::normalize(capsule1.tip - capsule1.base);
		vec_type line_end_offsetA = normA * capsule1.radius;
		vec_type aA = capsule1.base + line_end_offsetA;
		vec_type aB = capsule1.tip - line_end_offsetA;

		//capsule 2,
		vec_type normB = glm::normalize(capsule2.tip - capsule2.base);
		vec_type line_end_offsetB = normB * capsule2.radius;
		vec_type bA = capsule2.base + line_end_offsetB;
		vec_type bB = capsule2.tip - line_end_offsetB;

		//vectors between line endpoints
		vec_type v0 = bA - aA;
		vec_type v1 = bB - aA;
		vec_type v2 = bA - aB;
		vec_type v3 = bB - aB;

		//squred distnaces
		real_type d0 = glm::dot(v0, v0);
		real_type d1 = glm::dot(v1, v1);
		real_type d2 = glm::dot(v2, v2);
		real_type d3 = glm::dot(v3, v3);

		vec_type best_a;
		if (d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1)
		{
			best_a = aB;
		}
		else {
			best_a = aA;
		}
		real_type best_b = closest_point_on_line(best_a, td::geo::line<3, T, q>{ bA, bB });
		best_a = closest_point_on_line(best_b, geo::line<3, T, q>{aA, aB});

		vec_type peneration_normal = best_a - best_b;
		real_type len = glm::length(peneration_normal);
		peneration_normal /= len;
		real_type peneration_depth = capsule1.radius + capsule2.radius - len;
		bool intersects = peneration_depth > static_cast<real_type>(0);
		if (intersects) {
			std::get<intersect_normal>(intersection_data) = peneration_normal;
			std::get<intersect_penetration_depth>(intersection_data) = peneration_depth;
			std::get<intersect_point>(intersection_data) = best_a;
			return true;
		}
		return false;
	}

	//DDA algorithm for fast raycasting 



	//rect intersections
	template<typename T, glm::qualifier q>
	bool rect_point_intersection(const geo::rect<T, q>& rect, const glm::vec<2, T, q>& point)
	{
		return (point.x >= rect.position.x 
			&& point.y >= rect.position.y 
			&& point.x < rect.position.x + rect.size.x
			&& point.y < rect.position.y + rect.size.y);
	}


	template<typename T, glm::qualifier q>
	bool rect_rect_intersection(const geo::rect<T, q>& rect1, const geo::rect<T, q>& rect2)
	{
		return (rect1.position.x < rect2.position.x + rect2.size.x
			&& rect1.position.x + rect1.size.x > rect2.position.x
			&& rect1.position.y < rect2.position.y + rect2.size.y
			&& rect1.position.y + rect1.size.y > rect2.position.y);
	}
    
    template<typename T, glm::qualifier Q>
    bool test_sphere_capsule(const geo::sphere<T,Q>& sphere, const geo::capsule<T, Q>& capsule)
    {
            T dist2 = sq_distance(sphere.center, capsule.base, capsule.tip);
            T radius = sphere.radius + capsule.radius;
            return dist2 <= radius * radius;
    }
        
    template<typename T, size_t K>
    bool test_kdop_kdop(const geo::kdop<T, K>& kdop0, const geo::kdop<T, K>& kdop1)
    {
        //check if the intervals are interlapping
        constexpr const size_t k_2 = static_cast<size_t>(K/2);
        for(int i =0; i < k_2; i++)
        {
                if(kdop0.min[i] > kdop1.max[i] || kdop0.max[i] < kdop1.min[i]){
                    return false;
                }
        }
        //all intervals are intersecting so therefore, the kdop must be intersecting
        return true;
    }
    
}