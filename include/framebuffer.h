#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include "resource.h"



//hold a single framebuffer 
namespace td{
    namespace gpu{
    class fb : public td::gpu::resource
    {
        public:
            fb();
            virtual ~fb();
            virtual bool is_ok() const override;
            virtual void bind() const override;
            virtual void unbind() const override;
            
            
                        
    };
    }
}