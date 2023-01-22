#include "stream.h"

//stream functions
td::stream::stream()
{
    
}


td::stream::~stream()
{
    
}

bool td::stream::insert(object* obj)
{
    if(!obj) {
        //cannot insert null ?? log??
            return false;
    }
    auto pair = m_top_level_objs.insert(obj);
    return pair.second;
}
bool td::stream::remove(object* obj)
{
    if(!obj) return false;
    //if object does not exist no wam
    m_top_level_objs.erase(obj);
    return true;
}
void td::stream::remove_all()
{
    //removes the objects
    m_top_level_objs.clear();
}


bool td::stream::is_top_level(object* obj) const 
{
    auto iter = m_top_level_objs.find(obj);
    return (iter != m_top_level_objs.end());
}
