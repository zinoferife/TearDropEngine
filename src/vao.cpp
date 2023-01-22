#include "vao.h"


td::gpu::vao::vao()
{
    //create a glVertexArray
    glCreateVertexArrays(1, &m_res_handle);
}


td::gpu::vao::~vao()
{
    //delete the vertex array
    glDeleteVertexArrays(1, &m_res_handle);
}
void td::gpu::vao::bind() const
{
    //bind a vertex 
    glBindVertexArray(m_res_handle);
}
void td::gpu::vao::unbind() const
{
    //unbind
    glBindVertexArray(0);
}
