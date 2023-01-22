#pragma once
#include "geometry.h"
#include "ext_math.h"


#include <glm/geometric.hpp>


namespace td
{
	//distance from point to linear component
	template<glm::length_t d, typename T, glm::qualifier q>
	T distance(const glm::vec<d, T, q>& point, const td::geo::line<d, T, q>& line)
	{
		typedef glm::vec<d, T, q> vec_type;
		vec_type direction = glm::normalize((line[0] - line[1]));
		auto temp = glm::cross(direction, (line[0] - point));
		return glm::length(temp);

	}

	template<glm::length_t d, typename T, glm::qualifier q>
	T distance(const glm::vec<d, T, q>& point, const td::geo::ray<d, T, q>& ray)
	{
		auto temp = glm::cross(ray.direction, (ray.origin - point));
		return  glm::length(temp);
	}

	template<glm::length_t d, typename T, glm::qualifier q>
	T distance(const glm::vec<3, T, q>& point, const td::geo::plane<T, q>& plane)
	{
		return (glm::dot(plane.normal, point) - plane.constant);
	}

	
    
    template<glm::length_t d, typename T, glm::qualifier q>
    T sq_distance(const glm::vec<d,T, q>& point, const glm::vec<d, T, q>& l0, 
        const glm::vec<d, T, q>& l1)
    {
           typedef glm::vec<d, T, q> vec_t;
           vec_t ab = l1 - l0;
           vec_t ac = point - l0;
           vec_t bc = l1 - point;
           
           T e = glm::dot(ab, ac);
           if(e <= static_cast<T>(0.0)){
                return glm::dot(ac, ac);
           }
           T f = glm::dot(ab, ab);
           if(e >= f) return glm::dot(bc, bc);
           
           //handles the case where point projects to the line,
           return glm::dot(ac, ac) - (e * e) / f;
           
    }
    
    template<glm::length_t d, typename T, glm::qualifier q>
    T distance(const glm::vec<d, T, q>& point, const geo::plane<T, q>& pl)
    {
            T demom = glm::dot(pl.normal, pl.normal);
            return ((glm::dot(pl.normal, point) - pl.constant) / demom);
    }
}