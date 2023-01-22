#include <iostream>

#ifdef WINDOWS
#include <windows.h>
#include <GL/glew.h>
#include <gl/GL.h>
#endif


#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>


#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <chrono>

#include <fstream>
#include <filesystem>
#include <sstream>


#include "ext_math.h"
#include "intersection.h"
#include "type_traits.h"

#include "common_shapes.h"
#include "vao.h"
#include "texture.h"
#include "shader_program.h"
#include "gui.h"


namespace fs = std::filesystem;

void error_callback(int error, const char* description)
{
	spdlog::error("{:d}, {}", error, description);
}

void mouse_loaction(GLFWwindow* winodw, int x, int y)
{
    
}


td::gpu::vao_shared_ptr vao;
td::gpu::vao_shared_ptr vao2;
td::shapes::shape<float> sp;
td::gpu::texture_shared_ptr tex;
td::gpu::texture_shared_ptr tex2;

td::gpu::shader_program_shared_ptr program;
std::unique_ptr<td::gui> g_gui;
std::unique_ptr<td::test_win> t = std::make_unique<td::test_win>();

td::gpu::texture_shared_ptr setup_texture(const fs::path& path)
{
        if(!fs::exists(path)){
            spdlog::error("Path {} does not exists", path.string());
            return {};
        }
        auto tex = std::make_shared<td::gpu::texture>(td::gpu::texture::tex_t::tex2D);
        if(!tex){
            spdlog::error("Error allocating texture memory");
            return {};
        }
        //load the texture
        if(!tex->load_texture2D(path, 1)){
            spdlog::error("Cannot load texture at, {}", path.string());
            return {};
        }
        //set texture para
        tex->bind();
        tex->set_max_filter_para(td::gpu::texture::tex_para_t::tex_linear);
        tex->set_min_filter_para(td::gpu::texture::tex_para_t::tex_linear);
        tex->set_wrap_s(td::gpu::texture::tex_para_t::tex_wrap_repeat);
        tex->set_wrap_t(td::gpu::texture::tex_para_t::tex_wrap_repeat);
        tex->unbind();
        
        return tex;
}

void setup_shader(){
    program = std::make_shared<td::gpu::shader_program>();
    if(program){
          program->attach_shader("shaders/sphere_vert.txt", "VERTEX", td::gpu::shader_program::shader_t::vert);  
          program->attach_shader("shaders/sphere_frag.txt", "FRAGMENT", td::gpu::shader_program::shader_t::frag);
          program->link();
          program->clear_shaders();
    }else{
        spdlog::error("cannot create shader program");
    }
}


void startup()
{
    glViewport(0,0, 640, 480);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
	setup_shader();
    
	vao = std::make_shared<td::gpu::vao>();
    if(!vao){
        spdlog::error("Cannot create VAO on the heap");
        return;
    }
    vao->bind();
    tex = setup_texture("textures/earth2048.bmp");
    tex->bind_tex_unit(0);
    sp = td::shapes::sphere<float>(5.0, 36.0,18.0);
    vao->attach(sp.specs,sp.vert_buffer);
    sp.elem_buffer->bind();
    vao->unbind();
    
    vao2 = std::make_shared<td::gpu::vao>();
    if(!vao2) {
            spdlog::error("Cannot create VAO2 on heap");
            return;
    }
    vao2->bind();
    tex2 = setup_texture("textures/moon1024.bmp");
    vao2->attach(sp.specs, sp.vert_buffer);
    sp.elem_buffer->bind();
    vao2->unbind();
}




void setup_shader_data(float time)
{
        constexpr float radius = 30.0f;
        float camX = glm::cos(time) * radius;
        float camZ = glm::sin(time) * radius;
        glm::vec3 cam_pos(camX, 0.0f, camZ);
        glm::vec3 target = glm::vec3(0.0f, 0.0f, -10.0f);
        glm::mat4 view = glm::lookAt(cam_pos, target, glm::vec3(0.0,1.0,0.0));
        float aspect = 640.0f / 480.0f;
        glm::mat4 proj = glm::perspective<float>(glm::radians(45.0f), aspect, 0.1f, 100.0f);
              
        program->uniform_matrix("view_mat", view);         
        program->uniform_matrix("proj_mat", proj);         
       
        
        GLint tex_idx = glGetUniformLocation(program->get_res_handle(), "earth_tex");
        if(tex_idx == -1){
                spdlog::error("Invalid texture location in shader");
        }
        //load texture unit 0
        program->uniform_vec("zino", glm::vec4(1.0,1.0,1.0,1.0));
        
        glUniform1i(tex_idx, 0);
        
}


void test_shape_buffer(td::gpu::buffer_shared_ptr ptr)
{
        spdlog::info("Buffer use count: {:d}", ptr.use_count());
        typedef td::vertex_VNT<float> vnt_t;
        if(ptr){
            const size_t buffer_size = ptr->get_size();
            const size_t vertex_count = static_cast<size_t>(buffer_size / sizeof(vnt_t));
            //map buffer
            
            vnt_t* buf = reinterpret_cast<vnt_t*>(ptr->map_buffer_range(0, buffer_size, GL_MAP_READ_BIT));
            if(buf){
                    //successful mapped
                spdlog::info("Printing vertices in the buffer, size: {:d}", vertex_count);
                for(size_t i = 0; i < vertex_count; i++){
                   std::cout << fmt::format("Vertex: {:.4f} {:.4f} {:.4f}",buf[i].px, buf[i].py, buf[i].pz)
                    << "\n" <<
                    fmt::format("normal: {:.4f} {:.4f} {:.4f}",buf[i].nx, buf[i].ny, buf[i].nz) 
                    << "\n" <<
                    fmt::format("tex coord: {:.4f} {:.4f}",buf[i].tx, buf[i].ty) << std::endl; 
                }
            }
            ptr->unmap_buffer_range();
        }
}



void shutdown()
{
    
}

float current_time = 0.0f;
void render(float elapsed_time)
{
	const GLfloat color[] = { (GLfloat)glm::sin(current_time) * 0.5f + 0.5f, (GLfloat)glm::cos(current_time) * 0.5f + 0.5f, 0.0, 1.0f };
	const GLfloat clr_color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, clr_color);
    glClearDepthf(1.0);

	program->bind();
    current_time += elapsed_time;
    
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,-10.0f));
    program->uniform_matrix("model_mat", model);
    setup_shader_data(current_time);
    
    vao->bind();
    tex->bind_tex_unit(0);
    glDrawElements(GL_TRIANGLES, sp.elem_count, GL_UNSIGNED_INT, (void*)0);
    vao->unbind();
    
    
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f, -50.0f));
    program->uniform_matrix("model_mat", model);
    vao2->bind();
    tex2->bind_tex_unit(0);
    glDrawElements(GL_TRIANGLES, sp.elem_count, GL_UNSIGNED_INT, (void*)0);
    vao2->unbind();
    
    
    program->unbind();
	//spdlog::info("{:.2f} current time", current_time);
}


int main(int argc, char** argv)
{

	if (!glfwInit()) {
		spdlog::error("Cannot initialise GLFW");
		return -1;
	}
	glfwSetErrorCallback(error_callback);

	std::cout << fs::current_path().string() << std::endl;

	//creates the OpenGL context 
	GLFWwindow* window = glfwCreateWindow(640, 480, "TearDrop", nullptr, nullptr);
	if (!window) {
		spdlog::error("Cannot initialise and create window");
		return -1;
	}
	glfwMakeContextCurrent(window);
    glfwSwapInterval(1); //enable vsync
    g_gui = std::make_unique<td::gui>(window);
    g_gui->add_window(t.get());
	glewExperimental = true;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		spdlog::error("Cannot initialise glew");
	}
	startup();
	float elapsed_time = 0.0f;
	while (!glfwWindowShouldClose(window)) {
		auto start = std::chrono::high_resolution_clock::now();
		render(elapsed_time);
        
        g_gui->render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        g_gui->draw();
        
		glfwSwapBuffers(window);
		auto stop = std::chrono::high_resolution_clock::now();
		elapsed_time = std::chrono::duration<float, std::chrono::seconds::period>(stop - start).count();;
		glfwPollEvents();
	}
	shutdown();
	spdlog::info("Window is closing");
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}