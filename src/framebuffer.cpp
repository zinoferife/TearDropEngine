#include "framebuffer.h"
#include <spdlog/spdlog.h>

td::gpu::fb::fb()
{
    //create a framebuffer
    glCreateFramebuffers(1, &m_res_handle);
    if(glGetError() == GL_INVALID_OPERATION){
        //THROW INVALID ERROR 
        spdlog::error("Cannot create framebuffer, invalid operation in openGL");
        m_res_handle = 0;
        return;
    }
}
