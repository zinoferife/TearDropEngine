#pragma once
#include "object.h"


namespace td
{
    class controller : public object  {
        public:
            controller();
            controller(const std::string& name, std::uint64_t id);
            virtual ~controller();
            virtual bool update(double AppTime) {return true;}
        
    };
}