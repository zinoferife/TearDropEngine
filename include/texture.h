#pragma once

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>

#include <cstdint>
#include <type_traits>
#include <cassert>
#include <cstring>
#include <filesystem>


#include <spdlog/spdlog.h>


#include "resource.h"
#include <stb/stb_image.h>


namespace td
{
    
    
    
    namespace gpu{
    
    class texture : public resource {
        public:
        
        enum tex_t : GLenum {
                tex1D = GL_TEXTURE_1D,
                tex2D = GL_TEXTURE_2D,
                tex3D = GL_TEXTURE_3D,
                texCube = GL_TEXTURE_CUBE_MAP,
                texbuffer = GL_TEXTURE_BUFFER,
                tex1Darray = GL_TEXTURE_1D_ARRAY,
                tex2Darray = GL_TEXTURE_2D_ARRAY,
                texCubeArray = GL_TEXTURE_CUBE_MAP_ARRAY,
                tex2DMultiSample = GL_TEXTURE_2D_MULTISAMPLE,
                tex2DMultiSampleArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
        };
        
        enum tex_para_t : GLenum {
                tex_linear = GL_LINEAR,
                tex_nearest = GL_NEAREST,
                tex_mip_linear_linear = GL_LINEAR_MIPMAP_LINEAR,
                tex_mip_nearest_linear = GL_LINEAR_MIPMAP_NEAREST,
                tex_mip_nearest_nearest = GL_NEAREST_MIPMAP_NEAREST,
                tex_wrap_repeat = GL_REPEAT,
                tex_wrap_clamp = GL_CLAMP,
                tex_wrap_clamp_border = GL_CLAMP_TO_BORDER
        };
        
        //TODO: implement the proxy textures so that it can be used to query
        
        texture(tex_t tex_type); 
        virtual ~texture();
        
        
        virtual void bind() const override;
        virtual void unbind() const override;
        
        inline void set_bind_target(tex_t texture_target) {
                m_bind_target = texture_target;
        }
        
        //sets up the active unit and binds the texture
        void bind_to_active_unit(GLenum active_unit);
        
        
        bool load_texture2D(const std::filesystem::path& path, size_t levels = 0);
        bool load_texture3D(std::uint8_t* buffer, GLenum data_type, GLenum internal_format, size_t width, size_t height, size_t depth, size_t level = 0);
        bool load_texture2D(std::uint8_t* buffer, GLenum data_type, GLenum internal_format, size_t width, size_t height, size_t level = 0);
        bool load_texture1D(std::uint8_t* buffer, GLenum data_type, GLenum internal_format, size_t width, size_t level = 0);
        bool storage2D_multisample(std::uint32_t samples, GLenum internal_format, size_t width, size_t height, bool fixed_sample_locations);
        bool storage3D_multisample(std::uint32_t samples, GLenum internal_format, size_t width, size_t height, size_t depth, bool fixed_sample_locations);
        
        //
        inline void set_min_filter_para(tex_para_t para){
            glTexParameteri(m_bind_target, GL_TEXTURE_MIN_FILTER, para);
        }
        
        inline void set_max_filter_para(tex_para_t para){
            glTexParameteri(m_bind_target, GL_TEXTURE_MAG_FILTER, para);
        }
        
        inline void set_wrap_s(tex_para_t para){
                glTexParameteri(m_bind_target, GL_TEXTURE_WRAP_S, para);
        }
        
        inline void set_wrap_t(tex_para_t para){
            glTexParameteri(m_bind_target, GL_TEXTURE_WRAP_T, para);
        }
        
        inline void bind_tex_unit(GLuint unit){
                glBindTextureUnit(unit, m_res_handle);
        }
        
        void stroage1D(GLenum internal_format, size_t width, size_t levels = 0);
        void storage2D(GLenum internal_format, size_t width, size_t height, size_t levels = 0);
        void storage3D(GLenum internal_format, size_t width, size_t height, size_t depth, size_t levels = 0);
        
        void generate_mipmap() const;

        private:
        tex_t m_bind_target;
        tex_para_t m_tex_min_filter_para;
        tex_para_t m_tex_mag_filter_para;
        tex_para_t m_tex_wraps_para;
        tex_para_t m_tex_wrapt_para;
        tex_para_t m_tex_wrapq_para;
    };
    typedef std::shared_ptr<gpu::texture> texture_shared_ptr;
    typedef std::weak_ptr<gpu::texture> texture_weak_ptr;
    
    }
}