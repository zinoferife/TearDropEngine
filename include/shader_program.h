#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

#include <exception>

#include "resource.h"

#include <type_traits>
#include "type_traits.h"

#include <filesystem>
#include <memory>
#include <spdlog/spdlog.h>


namespace fs = std::filesystem;
namespace td
{
    namespace gpu{
    class shader_program : public resource
    {
        public:
            enum shader_t : GLenum {
              vert = GL_VERTEX_SHADER,
              frag = GL_FRAGMENT_SHADER,
              tess_eval = GL_TESS_EVALUATION_SHADER,
              tess_ctr = GL_TESS_CONTROL_SHADER,
              geo = GL_GEOMETRY_SHADER
            };
            
            class shader{
                public:
                shader(const std::string& shader_source, shader_t shader_type, bool is_file);
                ~shader();
                bool complie(const std::string& stage) const;
                std::string m_shader_source;
                shader_program::shader_t m_stype;
                //if shader source is file or a raw source
                bool m_isfile;
                GLuint m_shader_handle;
                
                
                
                //may not be needed
                bool operator==(const shader& rhs) const {
                        return m_shader_handle == rhs.m_shader_handle;
                }
                
                bool operator!=(const shader& rhs) const
                {
                    return m_shader_handle != rhs.m_shader_handle;
                }
            };
            
            shader_program();
            virtual ~shader_program();
            
            void attach_shader(const shader& s, const std::string& stage);
            void attach_shader(const fs::path& path, const std::string& stage, shader_t shader_type);
            bool link() const;
            void clear_shaders();
            
            //uniforms
            template<glm::length_t d, typename T, glm::qualifier q = glm::defaultp>
            bool uniform_matrix(const std::string& name, const glm::mat<d,d, T, q>& mat , size_t count = 1) const 
            {
                static_assert(std::is_floating_point_v<T>, "cannot load interger uniforms");
                static_assert(d >= 2 && d <= 4, "dimensions cannot be less than 2 0r greater than 4");
                const GLint i = glGetUniformLocation(m_res_handle, name.c_str());
                 if(i == -1) {
                        return false;
                }
                if constexpr(d == 4){
                            if constexpr(sizeof(T) == 4) 
                            glUniformMatrix4fv(i, count, GL_FALSE, glm::value_ptr(mat));
                            else if constexpr (sizeof(T) == 8)
                            glUniformMatrix4dv(m_res_handle, i, glm::value_ptr(mat));
                }else if constexpr (d == 3) {
                            if constexpr(sizeof(T) == 4) 
                            glUniformMatrix3fv(i, count, GL_FALSE, glm::value_ptr(mat));
                            else if constexpr (sizeof(T) == 8)
                            glUniformMatrix3dv(i, count, GL_FALSE, glm::value_ptr(mat));
                } else if constexpr(d == 2){
                            if constexpr(sizeof(T) == 4) 
                            glUniformMatrix2fv(i, count, GL_FALSE, glm::value_ptr(mat));
                            else if constexpr (sizeof(T) == 8)
                            glUniformMatrix2dv(i, count, GL_FALSE,  glm::value_ptr(mat));
                }
                return true;
                
            }
            
            template<glm::length_t d, typename T, glm::qualifier q = glm::defaultp>
            bool uniform_vec(const std::string& name, const glm::vec<d, T, q>& v, size_t count = 1) const {
                //loads a vector uniform
                const GLint i = glGetUniformLocation(m_res_handle, name.c_str());
                if(i == -1){
                        return false;
                }
                if constexpr (std::conjunction_v<
                   std::is_signed<T>, 
                   std::is_integral<T>>){
                    if constexpr (td::integral_equal<d, 4>::value){
                        glUniform4iv(i, count, glm::value_ptr(v));
                    }else if constexpr (td::integral_equal<d ,3>::value){
                         glUniform3iv(i, count, glm::value_ptr(v));
                    }else if constexpr (td::integral_equal<d ,2>::value){
                         glUniform2iv(i, count, glm::value_ptr(v));
                    }else{
                        return false;
                    }
                }else if constexpr (std::conjunction_v<
                std::is_unsigned<T>, 
                std::is_integral<T>>) {
                     if constexpr (td::integral_equal<d, 4>::value){
                        glUniform4uiv(i, count, glm::value_ptr(v));
                    }else if constexpr (td::integral_equal<d ,3>::value){
                         glUniform3uiv(i, count, glm::value_ptr(v));
                    }else if constexpr (td::integral_equal<d ,2>::value){
                         glUniform2uiv(i, count, glm::value_ptr(v));
                    }else{
                        return false;
                    }
                }else if constexpr(std::is_floating_point_v<T>){
                      if constexpr (td::integral_equal<d, 4>::value){
                        glUniform4fv(i, count, glm::value_ptr(v));
                    }else if constexpr (td::integral_equal<d ,3>::value){
                         glUniform3fv(i, count, glm::value_ptr(v));
                    }else if constexpr (td::integral_equal<d ,2>::value){
                         glUniform2fv(i, count, glm::value_ptr(v));
                    }else{
                        return false;
                    }
                }
                
            }
            
            template<typename T>
            bool uniform_value(const std::string& name, T value) const
            {
                const GLint i = glGetUniformLocation(m_res_handle, name.c_str());
                if(i != -1){
                    //no uniform name
                        return false;
                }
                //one dimentional uniforms
                if constexpr(std::is_integral_v<T>){
                        if constexpr(std::is_signed_v<T>){
                            glUniform1i(i, value);
                        }else if constexpr (std::is_unsigned_v<T>){
                                //
                            glUniform1ui(i, value);
                        }
                }else if constexpr (std::is_floating_point_v<T>)
                {  
                        if constexpr(td::integral_equal<sizeof(T), 4>::value){
                        //floating point 
                            glUniform1f(i, value);
                        }else {
                                glUniform1d(i, value);
                        }
                }else{
                        return false;
                }
                
                return true;
                
                
            }
            
            virtual void bind() const override;
            virtual void unbind() const override;
            
            
        private:
            //need to hold unifrom buffer
            
            //holds shaders
            std::vector<shader> m_shaders;
    };
        typedef std::shared_ptr<shader_program> shader_program_shared_ptr;
        typedef std::weak_ptr<shader_program> shader_program_weak_ptr;
    
    }
}