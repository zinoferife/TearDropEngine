
#include "shader_program.h"
#include <fstream>

static std::string read_from_file(const std::filesystem::path& path)
{
    if (!fs::exists(path)){
        spdlog::error("{} does not exists", path.string());
		return {};
	}

	std::ifstream in_file(path, std::ios::in);
	if (!in_file.is_open()) {
		spdlog::error("Could not open {}", path.string());
		return {};
	}

	std::ostringstream ss;
	ss << in_file.rdbuf();

	if (!ss.bad()){
		return ss.str();
	}
}

void td::gpu::shader_program::attach_shader(const shader& s, const std::string& stage)
{
    
    if(s.complie(stage)){
        glAttachShader(m_res_handle, s.m_shader_handle);
        m_shaders.emplace_back(std::move(s));
    }
}
bool td::gpu::shader_program::link() const
{
    GLint linked = 0;
    glLinkProgram(m_res_handle);
	glGetProgramiv(m_res_handle, GL_LINK_STATUS, &linked);
	if(!linked){
		spdlog::error("Could not link shader program");
		return false;
	}
    return true;
}
void td::gpu::shader_program::attach_shader(const fs::path& path, const std::string& stage, shader_t shader_type)
{
   m_shaders.emplace_back(path.string(),shader_type, true);
   auto& s = m_shaders.back();
   if(s.complie(stage)){
           glAttachShader(m_res_handle, s.m_shader_handle); 
   }else{
       //remove it if not compliled
        m_shaders.pop_back();
    }
}
td::gpu::shader_program::shader::shader(const std::string& shader_source, shader_t shader_type, bool is_file)
: m_shader_source(shader_source), m_stype(shader_type), m_isfile(is_file){
    m_shader_handle = glCreateShader(shader_type);
    
}
bool td::gpu::shader_program::shader::complie(const std::string& stage) const
{
    std::string shader_data;
    if(m_isfile){
        shader_data = read_from_file(fs::path(m_shader_source));
    }else{
        shader_data = std::move(m_shader_source);
    }
    if (shader_data.empty()) {
        spdlog::error("Failed to read in {} shader data from file", stage);
		return false;
    }
    const char* data = shader_data.c_str();
    glShaderSource(m_shader_handle, 1, &data, NULL);
    glCompileShader(m_shader_handle);
    GLint complied;
    glGetShaderiv(m_shader_handle, GL_COMPILE_STATUS, &complied);
    if (!complied)
    {
        GLint log_length;
        glGetShaderiv(m_shader_handle, GL_INFO_LOG_LENGTH, &log_length);
        char* info = new char[log_length];
        glGetShaderInfoLog(m_shader_handle, log_length, &log_length, info);
        spdlog::error("{}: {}", stage, info);
        return false;
    }
    return true;
}

td::gpu::shader_program::shader::~shader()
{
    glDeleteShader(m_shader_handle);
}
void td::gpu::shader_program::bind() const
{
    glUseProgram(m_res_handle);
}

void td::gpu::shader_program::unbind() const
{
    glUseProgram(0);
}
void td::gpu::shader_program::clear_shaders()
{
    m_shaders.clear();
}
td::gpu::shader_program::shader_program()
{
    m_res_handle = glCreateProgram();
}
td::gpu::shader_program::~shader_program()
{
    glDeleteProgram(m_res_handle);
}

