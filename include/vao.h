//represents a vertex buffer object
#pragma once
#include <GL/glew.h>
#include <GL/gl.h>

#include "resource.h"
#include "buffer.h"
#include "vertex_array.h"

#include <vector>

namespace td
{
        namespace gpu{
            
                class vao : public resource
                {
                  public:
                    static constexpr std::array<GLenum, 6> gl_types = 
                    {
                            GL_FLOAT, GL_DOUBLE, GL_BYTE, GL_UNSIGNED_BYTE, GL_INT, GL_UNSIGNED_INT
                    };
                    explicit vao();
                    virtual ~vao();
                    
                    virtual void bind() const override;
                    virtual void unbind() const override;
                    
                    //attaches a buffer to a vertex componet
                    template<typename vert_t>
                    void attach(const td::specification<vert_t>& spec,
                         buffer_shared_ptr buf, GLboolean norm = GL_FALSE)
                    {
                            //use the specification in the struct to attach a
                            //buffer to a vertex attribute
                            const auto& spec_values = spec();
                            buf->set_bind_target(td::gpu::buffer::buf_t::arraybuffer);
                            buf->bind();
                            for(size_t i = 0; i < spec_values.size(); i++)
                            {
                                //for each attribute in the spec call glVertexAttribPointer
                                const auto& spec_para = spec_values[i];
                                GLuint components = std::get<1>(spec_para);
                                GLenum a_type = gl_types[static_cast<size_t>(std::get<0>(spec_para))];
                                GLsizei stride = sizeof(vert_t);
                                GLuint offsetOF = std::get<2>(spec_para);
                                glEnableVertexAttribArray(i);
                                glVertexAttribPointer(i, components, a_type, norm, stride, (void*)offsetOF);
                              
                            }
                    }
                    
                    inline void enable_vertex_attrib(size_t idx) const
                    {
                        glEnableVertexAttribArray(idx);
                    }
                    
                    inline void disable_vertex_attrib(size_t idx) const{
                            glDisableVertexAttribArray(idx);
                    }
                    
                    //buffer management functions
                    
                    
                private:
                    //a vao holds a references to the buffers that it reads from
                };
                
                typedef std::shared_ptr<vao> vao_shared_ptr;
                typedef std::weak_ptr<vao> vao_weak_ptr;
        }
};
