//buffer represents the gpu memory
#include "buffer.h"

using namespace td::gpu;

void td::gpu::buffer::bind() const
{
    if(m_bind_target != GL_NONE){
        glBindBuffer(m_bind_target, m_res_handle);
    }
    
}
void td::gpu::buffer::unbind() const
{
    //bindto the default 
    glBindBuffer(m_bind_target, 0);
}

td::gpu::buffer::buffer(size_t size, GLbitfield bitfield)
: m_size(size), m_mapped_ptr(nullptr), m_buffer_flags(bitfield){
    glCreateBuffers(1, &m_res_handle);
    if(glIsBuffer(m_res_handle)){
            //if created, create stroage for the data
            // the size is in bytes
            //allow the buffer be later modified by a call to copy_sub_data
            bitfield |= GL_DYNAMIC_STORAGE_BIT;
            glNamedBufferStorage(m_res_handle, size, NULL, bitfield);
    }
    
}
 td::gpu::buffer::buffer(byte_t* data, size_t size, GLbitfield bitfield)
: m_size(size), m_mapped_ptr(nullptr), m_buffer_flags(bitfield)
{
    glCreateBuffers(1, &m_res_handle);
    if(glIsBuffer(m_res_handle)){
        glNamedBufferStorage(m_res_handle, size, data, bitfield);
    }
}

td::gpu::buffer::~buffer() 
{
        //destory the buffer
        //buffers are created on the heap, using the small object allocator
        glDeleteBuffers(1, &m_res_handle);
}

td::gpu::buffer::buffer(const td::gpu::buffer& rhs){
    glCreateBuffers(1, &m_res_handle);
    
    m_size = rhs.m_size;
    m_buffer_flags = rhs.m_buffer_flags;
    m_mapped_ptr = nullptr;
    m_bind_target = rhs.m_bind_target;
    
    glNamedBufferStorage(m_res_handle, rhs.m_size, NULL, rhs.m_buffer_flags.to_ulong());
    glCopyNamedBufferSubData(rhs.m_res_handle, m_res_handle, 0, 0, m_size);
}

td::gpu::buffer& td::gpu::buffer::operator =(const buffer& rhs)
{
    //assume that the resource is already created 
    m_size = rhs.m_size;
    m_buffer_flags = rhs.m_buffer_flags;
    if(m_mapped_ptr){
        glUnmapNamedBuffer(m_res_handle);
        m_mapped_ptr = nullptr;
    }
    
    m_bind_target = rhs.m_bind_target;
    glCopyNamedBufferSubData(rhs.m_res_handle, m_res_handle, 0,0, m_size);
  
    return (*this);
}


td::gpu::buffer::byte_t* td::gpu::buffer::map_buffer_range(size_t offset, size_t size, GLbitfield access)
{
    //test if the buffer was created with maping
    std::bitset<sizeof(GLbitfield) * 8> access_bits(access);
    if(m_buffer_flags.to_ulong() & access_bits.to_ulong())
    {
        m_mapped_ptr = static_cast<byte_t*>(glMapNamedBufferRange(m_res_handle,offset, size, access));
        if(!m_mapped_ptr){
            //something is wrong, use glGetError() to find error
            return NULL;
        }
        return m_mapped_ptr;
    }else{
            //buffer not mapped for writing or reading per the access specifier
    }
    
}
size_t td::gpu::buffer::write(const byte_t* data, size_t size)
{
    if(!data && !m_mapped_ptr){
            return 0;
    }
    //no need to store the range that was mapped, assume that size <= mapped_size;
    memcpy(m_mapped_ptr, data, size);
    return size;
}


//see if we can get the mapped region from the buffer body
size_t td::gpu::buffer::read(byte_t* data, size_t s)
{
    if(!data && !m_mapped_ptr){
            return 0;
    }
    memcpy(data, m_mapped_ptr, s);
    return s;
}
bool td::gpu::buffer::get_data(byte_t* data, size_t offset, size_t size)
{
    //gets data from the gpu buffer not mapped 
    glGetNamedBufferSubData(m_res_handle, offset, size, data);
    return (glGetError() != GL_INVALID_VALUE);
}
void td::gpu::buffer::copy_sub_data(size_t offset, size_t size, byte_t* data)
{
    if(!data && !m_buffer_flags.test(GL_DYNAMIC_STORAGE_BIT)) return;
    glNamedBufferSubData(m_res_handle, offset, size, data);
}

//makes the data visable to the buffer object in opengl
bool td::gpu::buffer::unmap_buffer_range()
{
        m_mapped_ptr = nullptr;
        return (glUnmapNamedBuffer(m_res_handle) == GL_TRUE);
}

void td::gpu::buffer::flush_mapped_range(size_t offset, size_t length)
{
    //tell opengl the part of the buffer that has valid data
    if(!m_buffer_flags.test(GL_MAP_FLUSH_EXPLICIT_BIT)){
        return;
    }
    glFlushMappedNamedBufferRange(m_res_handle, offset, length);
}
void td::gpu::buffer::invalidate_data(size_t offset, size_t size)
{
    //invalidate the data in the buffer, 
    glInvalidateBufferSubData(m_res_handle, offset, size);
}
void td::gpu::buffer::invalidate_data()
{
    //invalidate, the entire buffer
    glInvalidateBufferData(m_res_handle);
}
