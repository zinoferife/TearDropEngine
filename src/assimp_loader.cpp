#include "assimp_loader.h"

using namespace td;

const aiScene* load_model_data(const fs::path& data_path)
{
            static Assimp::Importer importer;
            const aiScene * scene = importer.ReadFile(data_path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);
            if(!scene){
                    //could not load data;
                    
            }
            
            return scene;
}