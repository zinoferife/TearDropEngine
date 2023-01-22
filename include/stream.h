#pragma once
#include <vector>
#include <set>

#include "reflect.h"
#include "object.h"

#include <filesystem>
#include <fstream>
#include <algorithm>
#include <iostream>


namespace fs = std::filesystem;
namespace td
{
    //derives from iostream, so that it can do both input and output
    //can use formatted writes and read according to the 
    
    class link{
      public:
            link() {}
            ~link() {}
    };
    
    
    class stream : public std::iostream {
    public:
                typedef std::set<object*> container_t;
                stream();
                ~stream();
                bool insert(object* obj);
                bool remove(object* obj);
                
                void remove_all();
                inline size_t get_object_count() const {
                        return m_top_level_objs.size();
                }
                
                bool is_top_level(object* obj) const;


                //memory loads and saves operation, the memeory buffer
                ///.. is owned by the application not the stream objects
                bool load(std::uint8_t* buffer, size_t size);
                bool save(std::uint8_t*& rbuffer, size_t& size);
                
                
                //file loads and save,
                bool load(const fs::path& filename);
                bool save(const fs::path& filename); //creates if does not exists
                
                
                //support for memory and disk usages
                size_t get_memory_used() const;
                size_t get_disk_used() const;
                
                
                //use the iostream operations on std::streambuf to read and write the data
                
    private:
            container_t m_top_level_objs;
            
                
    };
}