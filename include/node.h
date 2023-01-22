#include "spatial.h"

#include <vector>
#include <algorithm>
#include <type_traits>

#include <cassert>
#include <functional>
namespace td{
    class node : public spatial {
    public:
        typedef std::vector<std::shared_ptr<spatial>> container_t;
        typedef typename container_t::iterator iter_t;
        typedef typename container_t::const_iterator citer_t;
        node(size_t size = 1);
        virtual ~ node(); 
        
        template<typename T, typename... Args>
        void emplace_child_back(Args... args){
            static_assert(std::is_base_of_v<spatial, T>, "T is not a spatial type");
            auto& ref = m_children.emplace_back(std::make_shared<T>(args...));
            ref->set_parent(share<node>());
        }
        
        template<typename T>
        void emplace_child_back(){
            auto& ref = m_children.emplace_back(std::make_shared<T>());
            ref->set_parent(share<node>());
        }
        
        inline void push_child_back(std::shared_ptr<spatial> p){
            m_children.push_back(p);
            p->set_parent(share<node>());
        }
        
        //returns the previous pointer in the slot 
        //returns null if the slot holds nullptr
        inline std::shared_ptr<spatial> attach_child_at(size_t i, std::shared_ptr<spatial> p){
                assert(i < m_children.size() && "i is out of range");
                auto iter = std::next(m_children.begin(), i);
                if(!(*iter)) {
                    *iter = p;
                    return nullptr;
                }else{ 
                    auto ret = *iter;
                    *iter = p;
                    return ret;
                }
        }
        
        inline std::shared_ptr<spatial> detach_child_at(size_t i){
            assert(i < m_children.size() && "i is out of range");
            auto iter = std::next(m_children.begin(), i);
            if(iter == m_children.end()) return nullptr;
            auto p = *iter;
            //clear the slot
            *iter = nullptr;
            return p;
        }
        
        const std::shared_ptr<spatial>& operator[](size_t i) const{
                assert(i < m_children.size() && "i is out of range");
                return m_children[i];
        }
        
        std::shared_ptr<spatial>& operator[](size_t i) {
                assert(i < m_children.size() && "i is out of range");
                return m_children[i];
        }
        
        
        //a simple way to iterate through the children calling the functions
        template<typename R, typename... Args>
        void for_each(const std::function<R(iter_t iter,Args...)>& function, Args&&...args) const
        {
           for(auto iter = m_children.begin(); iter != m_children.end(); iter++)
           {
                function(iter, std::forward<Args>(args)...);
           }    
        }
        
        
        
        inline size_t get_count() const { return m_children.size(); }
        
        //expose the underlinging for other things lol might remove
        const container_t& get_children() const { return m_children;}
        
        
    protected:
        std::vector<std::shared_ptr<spatial>> m_children;
    };
}