#include <cstdint>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <type_traits>
#include <memory>
#include <iostream>
#include <vector>


#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include "vertex_array.h"
#include "buffer.h"
#include <glm/gtc/constants.hpp>

namespace td{
    namespace shapes{
            //all common shapes use a specific specification type
            //
            template<typename T, typename vertex_attr_t = td::vertex_VNT<T>>
            struct shape {
                using vert_attr_t = vertex_attr_t;
                
                td::gpu::buffer_shared_ptr vert_buffer;
                td::gpu::buffer_shared_ptr elem_buffer;
                size_t elem_count;
                td::specification<vertex_attr_t> specs;
            };
            
            template<typename T>   
            auto sphere(T radius, T sector_count, T stack_count) -> shape<T>
            {
                    static_assert(std::is_floating_point_v<T>, "T is not a floating point type");
                    //create the buffer
                    const size_t point_sampled = stack_count * (sector_count + 1);
                    const size_t vert_size = point_sampled * sizeof(typename shape<T>::vert_attr_t);
                    const size_t elem_size = point_sampled * sizeof(GLuint) * 6;
                    
                    shape<T> sphere_shape;
                    sphere_shape.vert_buffer = std::make_shared<td::gpu::buffer>(vert_size, GL_MAP_WRITE_BIT | 
                        GL_MAP_READ_BIT | GL_DYNAMIC_STORAGE_BIT);
                    sphere_shape.elem_buffer = std::make_shared<td::gpu::buffer>(elem_size, GL_MAP_READ_BIT | 
                        GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);
                    
                    sphere_shape.vert_buffer->set_bind_target(td::gpu::buffer::buf_t::arraybuffer);
                    sphere_shape.elem_buffer->set_bind_target(td::gpu::buffer::buf_t::elementbuffer);
                    
                    
                   vertex_VNT<T>* verts = (vertex_VNT<T>*)(sphere_shape.vert_buffer->map_buffer_range(0, vert_size, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT));
                   GLuint* elems =  (GLuint*)(sphere_shape.elem_buffer->map_buffer_range(0, elem_size, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT)); 
                    if(!elems  && !verts){
                            //cannot create gpu buffers, return null or log
                            spdlog::error("Cannot map gpu buffers for writing");
                            return td::shapes::shape<T>{};
                    }
                    
                    //generates a sphare, creates the buffer in memory
                    //angles are in pi
                    const T sector_step = (glm::two_pi<T>()) / sector_count;
                    const T stack_step = glm::pi<T>() / stack_count;
                    T stack_angle = static_cast<T>(0.0); 
                    T sector_angle = static_cast<T>(0.0);
                    const T len_inverse = static_cast<T>(1.0/ radius);
                    GLuint k1 = 0;
                    GLuint k2 = 0;
                    
                    size_t idx = 0;
                    for(std::uint32_t i = 0; i <= stack_count; i++)
                    {
                        stack_angle = glm::half_pi<T>() - i * stack_step;
                        T xz = radius * static_cast<T>(glm::cos(stack_angle));
                        T y = radius * static_cast<T>(glm::sin(stack_angle));
                    
                        for(std::uint32_t j = 0; j <= sector_count; j++){
                            sector_angle = j * sector_step;
                            T x = xz * static_cast<T>(glm::cos(sector_angle));
                            T z = xz * static_cast<T>(glm::sin(sector_angle));
                            
                            //positions
                            verts[idx].px = x;
                            verts[idx].py = y;
                            verts[idx].pz = z;
                            
                            //normals
                            verts[idx].nx = x * len_inverse;
                            verts[idx].ny = y * len_inverse;
                            verts[idx].nz = z * len_inverse;
                            
                            //texture cordinates
                            verts[idx].tx = ( static_cast<T>(1.0) - (T)j/ sector_count);
                            verts[idx].ty = ((T)i/ stack_count);
                            
                            idx++;
                    }
                        
                        
                    }
                    spdlog::info("number of vertices created: {:d}", idx);
                    //work out the elements idx
                    idx = 0;
                    for(std::uint32_t i = 0; i < stack_count; i++)
                    {
                            k1 = i * (sector_count + 1);
                            k2 = k1 + (sector_count + 1);
                            for(std::uint32_t j = 0; j < sector_count; j++, ++k1, ++k2)
                            {
                                    if(i != 0){
                                            elems[idx++] = k1;
                                            elems[idx++] = k2;
                                            elems[idx++] = k1 + 1;
                                    }
                                    
                                    if(i != (stack_count - 1)){
                                            elems[idx++] = k1 + 1;
                                            elems[idx++] = k2;
                                            elems[idx++] = k2 + 1; 
                                    }
                            }
                            
                    }
                sphere_shape.elem_count = idx;
                spdlog::info("Number of elements to draw: {:d}", idx);
                
                
                sphere_shape.vert_buffer->unmap_buffer_range();
                sphere_shape.elem_buffer->unmap_buffer_range();
                return sphere_shape;
                
            }
            
            
            template<typename T>
            auto box() -> shape<T>
            {
                shape<T> box_s;
                typedef vertex_VNT<T> vnt_t;
                constexpr size_t box_vert_buf_size = 8 * sizeof(vnt_t);
                constexpr size_t box_elem_buf_size = sizeof(GLuint) * 36;
                box_s.vert_buffer = std::make_shared<td::gpu::buffer>(box_vert_buf_size, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_DYNAMIC_STORAGE_BIT);
                box_s.elem_buffer = std::make_shared<td::gpu::buffer>(box_elem_buf_size, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_DYNAMIC_STORAGE_BIT);
                box_s.elem_count = 36;
                box_s.elem_buffer->set_bind_target(td::gpu::buffer::buf_t::elementbuffer);
                if(box_s.vert_buffer && box_s.elem_buffer){
                        box_s.vert_buffer->clear_data(static_cast<T>(0.0));
                        box_s.vert_buffer->clear_data(static_cast<GLuint>(0));
                        vnt_t* buf_vert = reinterpret_cast<vnt_t*>(box_s.vert_buffer->map_buffer_range(0, box_vert_buf_size, GL_MAP_WRITE_BIT));
                        GLuint* buf_elem = reinterpret_cast<GLuint*>(box_s.elem_buffer->map_buffer_range(0, box_elem_buf_size, GL_MAP_WRITE_BIT));
                        if(buf_vert && buf_elem){
                                //write in to the buffer
                                constexpr T vertices[] = { 
                                                 -1.0, -1.0, -1.0,
                                                  1.0, -1.0, -1.0,
                                                  1.0,  1.0, -1.0,
                                                 -1.0,  1.0, -1.0,
                                                 -1.0, -1.0,  1.0,
                                                  1.0, -1.0,  1.0,
                                                  1.0,  1.0,  1.0,
                                                 -1.0,  1.0,  1.0};
                            
                                constexpr GLuint elems[] = {
                                        0,1,2,
                                        2,3,0,
                                        1,5,6,
                                        6,2,1,
                                        7,6,5,
                                        5,4,7,
                                        4,0,3,
                                        3,7,4,
                                        4,5,1,
                                        1,0,4,
                                        3,2,6,
                                        6,7,3
                                }; 
                                //write the elems directly
                                box_s.elem_buffer->write((td::gpu::buffer::byte_t*)elems, sizeof(elems));
                                for(int i = 0, j = 0; i < 8; i++, j+= 3){
                                        buf_vert[i].px = vertices[j];
                                        buf_vert[i].py = vertices[j + 1];
                                        buf_vert[i].pz = vertices[j + 2];
                                }
                                 
                                 //for each triangles
                                 for(int i = 0; i < 36; i+= 3){
                                        const glm::vec3 a = {buf_vert[elems[i]].px, buf_vert[elems[i]].py, buf_vert[elems[i]].pz};
                                        const glm::vec3 b = {buf_vert[elems[i + 1]].px, buf_vert[elems[i + 1]].py, buf_vert[elems[i + 1]].pz};
                                        const glm::vec3 c = {buf_vert[elems[i + 2]].px, buf_vert[elems[i + 2]].py, buf_vert[elems[i + 2]].pz};
                                        
                                        const glm::vec3 normal = glm::normalize(glm::cross((b - a), (c - a)));
                                        //
                                        
                                        buf_vert[elems[i]].nx += normal.x; 
                                        buf_vert[elems[i]].ny += normal.y;
                                        buf_vert[elems[i]].nz += normal.z;
                                        
                                        buf_vert[elems[i + 1]].nx += normal.x; 
                                        buf_vert[elems[i + 1]].ny += normal.y;
                                        buf_vert[elems[i + 1]].nz += normal.z;
                                        
                                        buf_vert[elems[i + 2]].nx += normal.x; 
                                        buf_vert[elems[i + 2]].ny += normal.y;
                                        buf_vert[elems[i + 2]].nz += normal.z;
                                }
                                
                                //renormalise each normal for each vertex
                                for(int i = 0; i < 8; i++){
                                    glm::vec3 norm = glm::normalize(glm::vec3{buf_vert[i].nx, buf_vert[i].ny, buf_vert[i].nz});
                                    //write back
                                    buf_vert[i].nx = norm.x;
                                    buf_vert[i].ny = norm.y;
                                    buf_vert[i].nz = norm.z;
                                }
                                
                                
                                
                                box_s.vert_buffer->unmap_buffer_range();
                                box_s.elem_buffer->unmap_buffer_range();
                                return box_s;
                        }
                }
                
                //return empty shape, learn to use std::optional
                return shape<T>{};
                
            }
    }
    
} 