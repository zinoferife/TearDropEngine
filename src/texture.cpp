#include "texture.h"



namespace fs = std::filesystem;

td::gpu::texture::texture(tex_t tex_type)
{
    glCreateTextures(tex_type, 1, &m_res_handle);
    set_bind_target(tex_type);
}

td::gpu::texture::~texture(){
    glDeleteTextures(1, &m_res_handle);
}
void td::gpu::texture::bind() const
{
    //binds the texture
    glBindTexture(m_bind_target, m_res_handle);
}
void td::gpu::texture::unbind() const
{
    glBindTexture(m_bind_target, 0);
}
void td::gpu::texture::bind_to_active_unit(GLenum active_unit)
{
    glActiveTexture(active_unit);
    bind();
}

bool td::gpu::texture::load_texture2D(const std::filesystem::path& path, size_t levels)
{
    if(!fs::exists(path)) {
        //log-> texture2d file path does not exists
        spdlog::error("file at path {} does not exists", path.string());
        return false;
    }
    int x, y, n = 0;
    std::uint8_t* data = stbi_load(path.string().c_str(), &x, &y, &n, 0);
    if(!data){
        //could not load image file using stb_image
        spdlog::error("Cannot load image using stb_image, image corrupt or invalid");
        return false;
    }   
    
    //get internal format
    GLenum internalformat = GL_NONE;
    GLenum externalformat = GL_NONE;
    switch(n){
            case 1:
                internalformat = GL_R8;
                externalformat = GL_R;
                break;
            case 2:
                internalformat = GL_RG8;
                externalformat = GL_RG;
                break;
            case 3:
                internalformat = GL_RGB8;
                externalformat = GL_RGB;
                break;
            case 4: 
                internalformat = GL_RGBA8;
                externalformat = GL_RGBA;
                break;
            default:
                spdlog::error("Invalid internal format in texture");
                break;
    }
    if(internalformat == GL_NONE){
            if(data) stbi_image_free(data);
            return false;
    }
    
    //creates storage for the texture
    glTextureStorage2D(m_res_handle, levels, internalformat, x, y);
    
    // load image sub data
    glTextureSubImage2D(m_res_handle, 0, 0, 0, x, y, externalformat, GL_UNSIGNED_BYTE, data);
    
    //free memory, texture is in gpu memory now
    stbi_image_free(data);
    
    return true;
}
bool td::gpu::texture::load_texture2D(std::uint8_t* buffer, GLenum data_type, GLenum internal_format, size_t width, size_t height, size_t level)
{
    if(!buffer){
            spdlog::error("invalid buffer pointer");
            return false;
    }
    //assume that storage is already allocated for this texture...
    // load image sub data
    glTextureSubImage2D(m_res_handle, level, 0, 0, width, height, internal_format, data_type, buffer);
    return true;
}
bool td::gpu::texture::load_texture3D(std::uint8_t* buffer, GLenum data_type, GLenum internal_format, size_t width, size_t height, size_t depth, size_t level)
{
    if(!buffer) {
            spdlog::error("Cannot load 3d texture from buffer");
            return false;
    }
    glTextureSubImage3D(m_res_handle, level, 0,0,0, width, height, depth, internal_format, data_type, buffer);
    return true;
}

bool td::gpu::texture::load_texture1D(std::uint8_t* buffer, GLenum data_type, GLenum internal_format, size_t width, size_t level)
{
    if(!buffer) {
            spdlog::error("Cannot load 1d texture from buffer");
            return false;
    }
    glTextureSubImage1D(m_res_handle, level, 0, width, internal_format, data_type, buffer);
    return true;
}


//ask openGL to generate the mipmap
void td::gpu::texture::generate_mipmap() const {
    glGenerateTextureMipmap(m_res_handle);
}
