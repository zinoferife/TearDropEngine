#include <assimp/Importer.hpp>
#include <filesystem>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
namespace fs = std::filesystem;
//loads a model
namespace td{
    extern aiScene* load_model_data(const fs::path& path);
    
}