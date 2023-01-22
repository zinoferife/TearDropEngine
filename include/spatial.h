#include <spdlog/spdlog.h>
#include "object.h"

#include <memory>

namespace td{
    class spatial : public object {
    public:
        virtual ~spatial() {}
        inline const std::weak_ptr<spatial>& get_parent() const {
            return m_parent;
        }
        
        inline void set_parent(std::shared_ptr<spatial> p){
            m_parent = p;
        }
        
        inline bool has_parent() const {
                auto sp = m_parent.lock();
                return sp != nullptr;
        }
        
    protected:
        //the parent is an independent spatial object managed by a shared pointer
        //the parent pointer is a weak pointer to this object
        //means the object can delete itself when no reference to it
        //but the child referce to its parent wont prevent it from being deleted
        //also removes cyclic dependency from child and parent
        std::weak_ptr<spatial> m_parent;
        
        
        //default constructor is protected
        spatial() = default;
    };
}