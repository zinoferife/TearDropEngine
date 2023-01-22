//base class for gpu resources
#pragma once
#include <GL/gl.h>
#include <memory>

namespace td
{
    namespace gpu
    {
        //resources would manage its own data,
        //it is destoryed when there is nolonger any reference to it
        // 
      
        
        class resource : public std::enable_shared_from_this<resource>
        {
            public:
                typedef GLuint res_handle_t;
                virtual ~resource() {};
                const res_handle_t get_res_handle() const {return m_res_handle;}
                
                virtual void bind() const = 0;
                virtual void unbind() const = 0;
                
                virtual bool is_ok() const {return true; }
                inline std::shared_ptr<const resource> share() const 
                {
                        return this->shared_from_this();
                }
                
                inline std::shared_ptr<resource> share()
                {
                    return this->shared_from_this();
                }
                
                inline std::weak_ptr<const resource> share_weak() const 
                {
                        return this->shared_from_this();
                }
                
                inline std::weak_ptr<resource> share_weak() {
                        return this->shared_from_this();
                }
                
            protected:
                res_handle_t m_res_handle;
       
            };
    
        
        typedef std::shared_ptr<td::gpu::resource> res_shared_ptr;
        typedef std::weak_ptr<td::gpu::resource> res_weak_ptr;
        
        template<typename derived>
        std::shared_ptr<derived> resource_cast(const std::shared_ptr<resource>& res)
        {
                static_assert(std::is_base_of_v<resource, derived>, "Resource is not a base class of Derived");
                return std::dynamic_pointer_cast<derived>(res);
        }
    };
        
};
