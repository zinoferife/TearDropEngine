#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <type_traits>
#include "type_traits.h"

namespace td {
	//the transform 
	
	template<glm::length_t d, typename T, glm::qualifier q = glm::defaultp>
	class transform
	{
		//froms the transform for the engine
	public:
            static_assert(td::integral_equal<d, 3>::value || td::integral_equal<d, 2>::value, 
                "d should be either 2 dimenisons or 3 dimensions");
            static_assert(std::is_floating_point_v<T>, "transformation objects only work on floating point types");
            typedef glm::vec<d,T,q> vec_t;
            typedef glm::mat<d,d,T,q> mat_t;
            
            transform() 
            : rotate(1.0), translate(0.0), scale(1,0), is_identity(true),
                is_uniform_scale(false){}
            ~transform() {}
            
            static const transform identiy;
            
            template<glm::length_t f = 4, std::enable_if_t<(f == 4 && d == 3), int> = 0>
            glm::mat<f,f,T,q> to_homogeneous() const
            {
                    typedef glm::mat<f,f,T,q> mat_t;
                    //diagonal scale
                    glm::mat<d,d, T, q> S(scale);
                    auto R = rotate * S;
                    mat_t ret(R);
                    return glm::translate(ret, translate);
            }
            
            transform transpose() {
                    
            }
            
            transform inverse() {
                    
            }
            
            glm::vec<d,T,q> operator*(const glm::vec<d,T,q>& v) const
            {
                    
            }
            
            transform operator*(const transform& trans) const
            {
                    
            }
            
            //adds a vector to the translate 
            transform& operator+(const vec_t& tr){
                translate += tr;
                return (*this);
            }
            
    public:
            void rotation(const glm::mat<d,d,T,q>& rot){
                if(is_identity)is_identity = false;
                rotate = rot;
            }
            const glm::mat<d,d,T,q>& rotation() const {
                return rotate;
            }
            
            glm::mat<d,d,T,q>& rotation(){
                return rotate;
            }
            
            void translation(const vec_t& trans){
                if(is_identity) is_identity = false;
                translate = trans;
            }
            
            const vec_t& translaton() const {
                    return translate;
            }
            
            vec_t& translaton(){
                    return translate;
            }
            
            void scale_(const vec_t& s) {
                    if(is_uniform_scale) is_uniform_scale = false;
                    if(is_identity) is_identity = false;
                    scale = s;
            }
            
            const vec_t& scale_() const {
                    return  scale;
            }
            
            vec_t& scale_() {
                    return  scale;
            }
            
	private:
		//to
        glm::vec<d, T, q> translate;
        glm::vec<d, T, q> scale;
        glm::mat<d,d, T, q> rotate;
        bool is_identity;
        bool is_uniform_scale;
        
	};



}