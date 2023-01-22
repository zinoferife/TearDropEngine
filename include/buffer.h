//represents a buffer in the gpu memory, that is a memory allocation
#pragma once


#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>

#include <cstdint>
#include <type_traits>
#include <cassert>
#include <bitset>
#include <cstring>

#include "resource.h"
namespace td
{
    namespace gpu
    {
        class buffer : public resource
        {
        public:
                typedef std::uint8_t byte_t;
                enum buf_t : GLenum {
                        arraybuffer = GL_ARRAY_BUFFER,
                        uniformbuffer = GL_UNIFORM_BUFFER,
                        texturebuffer = GL_TEXTURE_BUFFER,
                        shaderbuffer = GL_SHADER_STORAGE_BUFFER,
                        elementbuffer = GL_ELEMENT_ARRAY_BUFFER,
                        packpixelbuffer = GL_PIXEL_PACK_BUFFER,
                        unpackpixelbuffer = GL_PIXEL_UNPACK_BUFFER
                };
                
                explicit buffer(size_t size, GLbitfield bitfield);
                explicit buffer(byte_t* data, size_t size, GLbitfield bitfield);
                explicit buffer(GLenum bind_target, size_t size, GLbitfield bitfield);
                explicit buffer(GLenum bind_target, byte_t* data, size_t size, 
                    GLbitfield bitfield);
                    
                virtual ~buffer();    
                
                //handle copy and assignment
                buffer(const buffer& rhs);
                buffer& operator=(const buffer& rhs);
                
                
                //handle move and move assignment operations
                buffer(const buffer&& rhs);
                buffer& operator=(const buffer&& rhs);
                
                
                //shaing the buffer
                virtual inline std::shared_ptr<const buffer> share() const{
                        return std::dynamic_pointer_cast<const buffer, const resource>(resource::share());
                }
                virtual inline std::weak_ptr<const buffer> share_weak() const{
                        return std::dynamic_pointer_cast<const buffer, const resource>(resource::share());
                }
                
                void copy_sub_data(size_t offset, size_t size, byte_t* data);
                
                
                //T can be any of the arithmetic types
                template<typename T>
                void clear_data(T clear_data)
                {
                        static_assert( std::is_arithmetic_v<T> ,"T is not an arithmetic type");
                        constexpr const size_t size = sizeof(T);
                        //derive format and type from template type information
                        GLenum format;
                        GLenum type;
                        if constexpr(std::is_floating_point_v<T>){
                            if constexpr(size == 4 || size == 8)
                               type = GL_FLOAT;
                               format = GL_R32F;
                        }else if constexpr(std::is_integral_v<T>){
                                if constexpr (std::is_signed_v<T>){
                                    if constexpr(size == 4){
                                    type = GL_INT;
                                    format = GL_R32I;
                                    }else if constexpr(size == 2){
                                            type = GL_SHORT;
                                            format = GL_R16I;
                                    }else if constexpr(size == 1){  
                                        type = GL_BYTE;
                                        format = GL_R8I;
                                    }
                                }else{
                                       if constexpr(size == 4){
                                    type = GL_UNSIGNED_INT;
                                    format = GL_R32UI;
                                    }else if constexpr(size == 2){
                                            type = GL_UNSIGNED_SHORT;
                                            format = GL_R16UI;
                                    }else if constexpr(size == 1){  
                                        type = GL_UNSIGNED_BYTE;
                                        format = GL_R8UI;
                                    }
                                }
                        }
                        glClearNamedBufferData(m_res_handle,GL_R8UI, format, type , &clear_data);
                }
                
                template<typename T>
                void clear_sub_data(size_t offset, size_t size, T clear_data)
                {
                        static_assert(std::is_arithmetic_v<T>, "T is not arithmetic type");
                        constexpr const size_t s = sizeof(T);
                        
                        GLenum format = GL_R;
                        GLenum type;
                        if constexpr(s == 4 && std::is_floating_point_v<T>){
                               type = GL_FLOAT;
                        }else if constexpr(s == 4 && std::is_integral_v<T>){
                                if constexpr (std::is_signed_v<T>){
                                    type = GL_INT;
                                }else{
                                    type = GL_UNSIGNED_INT;
                                }
                        }
                        glClearNamedBufferSubData(m_res_handle, format, offset, size, format, type, &clear_data);
                }
                inline void set_bind_target(buf_t bind_target){
                        m_bind_target = bind_target;
                }
                
                inline size_t get_size() const {return m_size;}
                
                
                virtual void bind() const override;
                virtual void unbind() const override;
                
                
                inline void bind_target(GLenum target) const {
                    glBindBuffer(target, m_res_handle);
                }
                //returns true if the map was successful
                byte_t* map_buffer_range(size_t offset, size_t size, GLbitfield access);
                
                //read and write the buffer, through the mapped pointer
                //if the buffer is not mapped the read and write would fail failed
                //returns the number of bytes was written or read from the buffer
                //write as much as possible, disraged the rest, return the size that was written
                size_t write(const byte_t* data, size_t size);
                size_t read(byte_t* data, size_t read);
                
                //get data from the buffer that is not mapped
                //useful for transform feedback or compute shading
                bool get_data(byte_t* data, size_t offset, size_t size);
        
        
                void flush_mapped_range(size_t offset, size_t size);
                void invalidate_data(size_t offset, size_t size);
                void invalidate_data();
                bool unmap_buffer_range();
                
            private:
                buf_t m_bind_target;
                size_t m_size;
                byte_t* m_mapped_ptr;
                std::bitset<sizeof(GLbitfield) * 8> m_buffer_flags;
        };
        
        typedef std::shared_ptr<buffer> buffer_shared_ptr;
        typedef std::weak_ptr<buffer> buffer_weak_ptr;
    };
};