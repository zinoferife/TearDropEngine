#include <glm/glm.hpp>
#include "geometry.h"


//functions to calulate the closest points to geometric objects
//TODO: static_assert all the funtions that take d template parameter d, dimension of 1 is invalid
namespace td{
    template<typename T, glm::length_t D, glm::qualifier Q = glm::defaultp>
    glm::vec<D, T, Q> closest_point_on_plane(const geo::plane<T, Q>& pl, const glm::vec<D, T, Q>& point)
    {
        static_assert(D == 3, "Point must be a 3 dimensional point");
        static_assert(std::is_floating_point_v<T>, "T must be floating point type");
        T t = glm::dot(pl.normal, point) - pl.constant;
        return (point + (pl.normal * t));
    }
        
    template<glm::length_t d, typename T, glm::qualifier q = glm::defaultp>
	glm::vec<d, T, q> closest_point_on_line(const glm::vec<d, T, q>& point, const geo::line<d, T, q>& line)
	{
        static_assert(std::is_floating_point_v<T>, "T is uspposed to be a floating point type");
		auto ab = line[1] - line[0];
		T t = glm::dot(point - line[0], ab) / glm::dot(ab, ab);
		return (line[0] + ab * td::math::saturate(t));
	}
    
    template<glm::length_t d, typename T, glm::qualifier q = glm::defaultp>
    glm::vec<d, T, q> closest_point_on_line_def_div(const glm::vec<d, T, q>& point,
        const geo::line<d, T, q>& l)
    {
       static_assert(std::is_floating_point_v<T>, "T is supposed to be a floating point type");
       auto ab = l[1] - l[0];
       T t = glm::dot(point - l[0], ab);
       if(t <= static_cast<T>(0.0)){
            return l[0];
            t = static_cast<T>(0.0);
       }else{ 
           T demom = glm::dot(ab, ab);
           if(t >= demom){
               return l[1];
               t = static_cast<T>(1.0);
           }else{
                //do the division
                t = t / demom;
                return l[0] + t * ab;
            }
        }
    }
    
    
    
    template<glm::length_t d, typename T, glm::qualifier q = glm::defaultp>
    glm::vec<d, T, q> closest_point_on_aabb(const glm::vec<d, T, q>& point, const geo::aabb<d, T, q>& box){
            typedef glm::vec<d, T, q> vec_t;
            vec_t p = point;
            for(size_t i = 0; i < d; i++){
                    //for each axis clamp point to externts
                    
            }
    
    }
    
    template<glm::length_t d, typename T, glm::qualifier q = glm::defaultp>
    glm::vec<d, T, q> closest_point_on_ray(const glm::vec<d,T,q>& point, const geo::ray<d, T ,q>& r){
            //divid might not be needed because the direction is normalised
            T t = glm::dot(point - r.origin, r.direction) / glm::dot(r.direction, r.direction); 
            t = glm::max(t, static_cast<T>(0.0));
            return (r.origin + t * r.direction);
    }
    
    template<glm::length_t d, typename T, glm::qualifier q = glm::defaultp>
    glm::vec<d, T, q> closest_point_on_triangle(
            const glm::vec<d, T, q>& p,
            const glm::vec<d, T, q>& a,
            const glm::vec<d, T, q>& b,
            const glm::vec<d, T, q>& c)
    {
        //projecting 
        static_assert(std::is_floating_point_v<T>, "T is supposed to be a floating point type");
        typedef glm::vec<d, T, q> vec_t;
        vec_t ab = b - a;
        vec_t ac = c - a;
        vec_t ap = p - a;
        
        constexpr T zero = static_cast<T>(0.0);
        T d1 = glm::dot(ab, ap);
        T d2 = glm::dot(ac, ap);
        if(d1 <= zero && d2 <= zero) return a; //barycentric coordinate (1,0,0)
        
        vec_t bp = p - b;
        T d3 = glm::dot(ab, bp);
        T d4 = glm::dot(ac, bp);
        if(d3 <= zero && d4 <= d3) return b; // barycentric coordinate (0,1,0);
        
        T vc = d1 * d4 - d3 * d2;
        if(vc <= zero && d1 >= zero && d3 <= zero){
                T v = d1/ (d1 - d3);
                return a * v * ab; //barycentric coorinate (1-v, v, 0)
        }
        
        vec_t cp  = p - c;
        T d5 = glm::dot(ab, cp);
        T d6 = glm::dot(ac, cp);
        if(d6 >= zero && d5 <= d6) return c; // barycentric coordinate (0,0,1)
        
        T vb = d5* d2 - d1 * d6;
        if (vb <= zero && d2 >= zero && d6 <= zero) {
             T w = d2 / (d2 - d6);
            return a + w * ac; // barycentric coordinates (1-w,0,w)
        }
        
        T va = d3 * d6 - d5 * d4;
        if (va <= zero && (d4 - d3) >= zero && (d5 - d6) >= zero) {
                T w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
                return b + w * (c - b); // barycentric coordinates (0,1-w,w)
        }

        T denom = static_cast<T>(1.0) / (va + vb + vc);
        T v = vb * denom;
        T w = vc * denom;
        return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
        
    }
    
    template<glm::length_t d, typename T, glm::qualifier q = glm::defaultp>
    void closest_point_line_on_line(const td::geo::line<d, T, q>& l1,
    const td::geo::line<d, T, q>& l2, glm::vec<d, T, q>& p1, 
    glm::vec<d,T, q>& p2){
            static_assert(std::is_floating_point_v<T>, "T should be a floating point type");
            typedef glm::vec<d, T, q> vec_t;
            constexpr T zero = static_cast<T>(0.0);
            constexpr T one = static_cast<T>(1.0);
            const vec_t d1 = l1.p1 - l1.p0;
            const vec_t d2 = l2.p1 - l2.p0;
            
            vec_t r = l1.p0 - l2.p0;
            const T a = glm::dot(d1, d1);
            const T e = glm::dot(d2, d2);
            const T f = glm::dot(d2, r);
            
            if(a <= glm::epsilon<T>() && e <= glm::epsilon<T>()){
                    //linee segments degenerate to points
                    p1 = l1.p0;
                    p2 = l2.p0;
                    return;
            }
            T s = zero;
            T t = zero;
            
            if(a <= glm::epsilon<T>()){
                s = zero;
                t = f/e; 
                t = glm::clamp(t, zero, one);
            }else {
                const T c = glm::dot(d1, r); 
                if(e <= glm::epsilon<T>()){
                        //second segment degerates to a point
                        t = zero;
                        s = glm::clamp(- c / a, zero, one);
                }else {
                        //general non degenerate case
                        const T b = glm::dot(d1, d2);
                        const T denom = a * e - b * b;
                        if(denom != zero){
                                s = glm::clamp((b* f - c * e )/denom, zero, one);
                        }else s = zero;
                        t = (b*s + f) / e;
                        if (t < zero) {
                                    t = zero;
                                    s = glm::clamp(-c / a, zero, one);
                        } else if (t > one) {
                                    t = one;
                                    s = glm::clamp((b - c) / a, zero, one);
                        }
                }
            }
            p1 = l1.p0 + d1 * s;
            p2 = l2.p0 + d2 * t;
    }
    
    
}
