#pragma once

#include <string>
#include <glm/glm.hpp>   // Para glm::vec3
#include "Model.h"       // Para ModelMesh

class SceneSerializer
{
public:
    // Guarda la escena en carpeta libraryDir
    static bool SaveScene(const std::string& libraryDir);

    // Carga la escena desde libraryDir
    static bool LoadScene(const std::string& libraryDir);

private:
    // Helpers para guardar/leer malla
    static bool WriteMeshBinary(const std::string& path, const ModelMesh& mesh);
    static bool ReadMeshBinary(const std::string& path, ModelMesh& mesh);

    // Helpers JSON mínimos
    static std::string ExtractString(const std::string& blob, const std::string& key);
    static glm::vec3 ExtractVec3(const std::string& blob, const std::string& key);
    static int ExtractInt(const std::string& blob, const std::string& key);
};
