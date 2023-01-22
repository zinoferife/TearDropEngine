//imgui...
#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <vector>

static const char* glsl_version = "#version 130";
namespace td
{
    class gui{
    public:
    
        //form the bases of a window in imgui
        class win{
                //represents a single window in imgui contex
        public:
                win() {}
                virtual ~win() {}
                virtual void create() = 0;
        };
        
        
        gui(GLFWwindow* window);
        ~gui();
        inline void toggle_style() {
                m_style = !m_style;
        }
        
        void render() const;
        void draw() const;
        
        
        void add_window(win* w) {
                m_windows.emplace_back(w);
        }
        
        void remove_back() {
            m_windows.pop_back();
        }
        
    private:
        bool m_style = false;
        std::vector<win*> m_windows;
        
    };
    
    class test_win : public gui::win {
    public:
            virtual ~test_win() {}
            virtual void create() override;
    };
    
}

