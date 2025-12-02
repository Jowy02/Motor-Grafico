#include "SceneSerializer.h"
#include "Application.h"
#include "Scene.h"
#include "Model.h"
#include "Texture.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <windows.h>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>


static uint32_t GenerateUID() {
    static bool initialized = false;
    if (!initialized) {
        std::srand((unsigned int)std::time(nullptr));
        initialized = true;
    }
    return (uint32_t)(std::rand() & 0xFFFFFFFF);
}

bool SceneSerializer::WriteMeshBinary(const std::string& path, const ModelMesh& mesh)
{
    std::ofstream ofs(path, std::ios::binary);
    if (!ofs) return false;
    uint32_t vcount = (uint32_t)mesh.positionsLocal.size();
    uint32_t icount = (uint32_t)mesh.indices.size();
    ofs.write((char*)&vcount, sizeof(vcount));
    for (auto& p : mesh.positionsLocal)
    {
        ofs.write((char*)&p.x, sizeof(float));
        ofs.write((char*)&p.y, sizeof(float));
        ofs.write((char*)&p.z, sizeof(float));
    }
    ofs.write((char*)&icount, sizeof(icount));
    for (auto& idx : mesh.indices)
        ofs.write((char*)&idx, sizeof(uint32_t));
    return true;
}

bool SceneSerializer::ReadMeshBinary(const std::string& path, ModelMesh& mesh)
{
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) return false;
    uint32_t vcount = 0, icount = 0;
    ifs.read((char*)&vcount, sizeof(vcount));
    mesh.positionsLocal.resize(vcount);
    for (uint32_t i = 0; i < vcount; ++i)
    {
        float x, y, z;
        ifs.read((char*)&x, sizeof(float));
        ifs.read((char*)&y, sizeof(float));
        ifs.read((char*)&z, sizeof(float));
        mesh.positionsLocal[i] = glm::vec3(x, y, z);
    }
    ifs.read((char*)&icount, sizeof(icount));
    mesh.indices.resize(icount);
    for (uint32_t i = 0; i < icount; ++i)
    {
        uint32_t idx;
        ifs.read((char*)&idx, sizeof(uint32_t));
        mesh.indices[i] = idx;
    }
    mesh.indexCount = icount;
    return true;
}


// ---------------- Directory helper ----------------
static void CreateFolder(const std::string& path)
{
    CreateDirectory(path.c_str(), NULL);
}

// ---------------- SceneSerializer ----------------
bool SceneSerializer::SaveScene(const std::string& libraryDir)
{
    Scene* scene = Application::GetInstance().scene.get();

    // Crear carpetas necesarias
    CreateFolder(libraryDir);
    CreateFolder(libraryDir + "\\Meshes");
    CreateFolder(libraryDir + "\\Textures");
    CreateFolder(libraryDir + "\\FBX");

    std::ostringstream json;
    json << "{\n\"GameObjects\": [\n";

    for (size_t i = 0; i < scene->models.size(); ++i)
    {
        Model& m = scene->models[i];
        if (m.modelId == 0) m.modelId = GenerateUID();

        std::string meshFileName;
        if (!m.Mmesh.positionsLocal.empty())
        {
            meshFileName = "Meshes/mesh_" + std::to_string(m.modelId) + ".mesh";
            WriteMeshBinary(libraryDir + "\\" + meshFileName, m.Mmesh);
        }

        std::string fbxFileName;
        if (!m.fbxPath.empty())
        {
            size_t pos = m.fbxPath.find_last_of("\\/");
            fbxFileName = m.fbxPath.substr(pos + 1);

            // Copiar FBX al directorio de la biblioteca
            std::ifstream src(m.fbxPath, std::ios::binary);
            if (src)
            {
                CreateFolder(libraryDir + "\\FBX");
                std::ofstream dst(libraryDir + "\\FBX\\" + fbxFileName, std::ios::binary);
                dst << src.rdbuf();
            }
        }

        // En JSON
        if (!fbxFileName.empty())
            json << "    \"FBX\": \"" << fbxFileName << "\",\n";


        std::string texFileName;
        if (!m.texturePath.empty())
        {
            size_t pos = m.texturePath.find_last_of("\\/");
            texFileName = m.texturePath.substr(pos + 1);

            std::ifstream src(m.texturePath, std::ios::binary);
            if (src)
            {
                std::ofstream dst(libraryDir + "\\Textures\\" + texFileName, std::ios::binary);
                dst << src.rdbuf();
            }
        }

        // JSON
        json << "  {\n";
        json << "    \"ModelId\": " << m.modelId << ",\n";
        json << "    \"ParentID\": " << (m.isChild ? m.ParentID : -1) << ",\n";
        json << "    \"Name\": \"" << m.name << "\",\n";
        json << "    \"Position\": [" << m.position.x << "," << m.position.y << "," << m.position.z << "],\n";
        json << "    \"Scale\": [" << m.scale.x << "," << m.scale.y << "," << m.scale.z << "],\n";
        json << "    \"Rotation\": [" << m.rotation.x << "," << m.rotation.y << "," << m.rotation.z << "],\n";
        json << "    \"Mesh\": \"" << meshFileName << "\",\n";
        json << "    \"FBX\": \"" << fbxFileName << "\",\n";
        json << "    \"Texture\": \"" << texFileName << "\"\n";
        json << "  }";

        if (i < scene->models.size() - 1)
            json << ",";
        json << "\n";
    }

    json << "]\n}";
    std::ofstream(libraryDir + "\\scene.json") << json.str();
    return true;
}

bool SceneSerializer::LoadScene(const std::string& libraryDir)
{
    Scene* scene = Application::GetInstance().scene.get();

    for (auto& m : scene->models) m.CleanUp();
    scene->models.clear();

    std::ifstream file(libraryDir + "\\scene.json");
    if (!file) return false;

    std::stringstream ss;
    ss << file.rdbuf();
    std::string json = ss.str();

    size_t pos = 0;
    while ((pos = json.find("{", pos)) != std::string::npos)
    {
        size_t end = json.find("}", pos);
        if (end == std::string::npos) break;

        std::string block = json.substr(pos, end - pos + 1);
        Model m("NULL");

        // ModelId
        size_t idPos = block.find("ModelId");
        if (idPos != std::string::npos)
            m.modelId = std::stoi(block.substr(block.find(":", idPos) + 1));

        // Name
        size_t nPos = block.find("Name");
        if (nPos != std::string::npos)
        {
            size_t q1 = block.find("\"", nPos + 4);
            size_t q2 = block.find("\"", q1 + 1);
            m.name = block.substr(q1 + 1, q2 - q1 - 1);
        }

        // Position
        size_t pPos = block.find("Position");
        if (pPos != std::string::npos)
        {
            size_t b1 = block.find("[", pPos);
            size_t b2 = block.find("]", b1);
            std::string val = block.substr(b1 + 1, b2 - b1 - 1);
            std::replace(val.begin(), val.end(), ',', ' ');
            std::istringstream iss(val);
            iss >> m.position.x >> m.position.y >> m.position.z;
        }

        // Scale
        size_t sPos = block.find("Scale");
        if (sPos != std::string::npos)
        {
            size_t b1 = block.find("[", sPos);
            size_t b2 = block.find("]", b1);
            std::string val = block.substr(b1 + 1, b2 - b1 - 1);
            std::replace(val.begin(), val.end(), ',', ' ');
            std::istringstream iss(val);
            iss >> m.scale.x >> m.scale.y >> m.scale.z;
        }

        // Rotation
        size_t rPos = block.find("Rotation");
        if (rPos != std::string::npos)
        {
            size_t b1 = block.find("[", rPos);
            size_t b2 = block.find("]", b1);
            std::string val = block.substr(b1 + 1, b2 - b1 - 1);
            std::replace(val.begin(), val.end(), ',', ' ');
            std::istringstream iss(val);
            iss >> m.rotation.x >> m.rotation.y >> m.rotation.z;
        }

        // Mesh
        size_t meshPos = block.find("Mesh");
        if (meshPos != std::string::npos)
        {
            size_t q1 = block.find("\"", meshPos);
            size_t q2 = block.find("\"", q1 + 1);
            std::string meshFile = block.substr(q1 + 1, q2 - q1 - 1);
            if (!meshFile.empty() && ReadMeshBinary(libraryDir + "\\" + meshFile, m.Mmesh))
            {
                m.UpdateTransform();
            }
        }

        // FBX
        size_t fbxPos = block.find("FBX");
        if (fbxPos != std::string::npos)
        {
            size_t q1 = block.find("\"", fbxPos);
            size_t q2 = block.find("\"", q1 + 1);
            if (q1 != std::string::npos && q2 != std::string::npos)
            {
                std::string fbxFile = block.substr(q1 + 1, q2 - q1 - 1);
                if (!fbxFile.empty())
                {
                    std::string fullPath = libraryDir + "\\FBX\\" + fbxFile;
                    if (std::ifstream(fullPath))
                    {
                        m.fbxPath = fullPath;
                        m.loadModel(fullPath);
                    }
                    else
                    {
                        std::cerr << "FBX not found: " << fullPath << "\n";
                    }
                }
            }
        }

        // Texture
        size_t texPos = block.find("Texture");
        if (texPos != std::string::npos)
        {
            size_t q1 = block.find("\"", texPos);
            size_t q2 = block.find("\"", q1 + 1);
            std::string texFile = block.substr(q1 + 1, q2 - q1 - 1);

            if (!texFile.empty())
            {
                std::string fullPath = libraryDir + "\\Textures\\" + texFile;
                if (std::ifstream(fullPath))
                {
                    Texture* t = new Texture(fullPath.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
                    m.Mmesh.texture = t;
                    m.texturePath = fullPath;
                }
            }
        }

        scene->models.push_back(m);
        pos = end + 1;
    }

    // Reconstruir jerarquía parent-child
    for (auto& m : scene->models)
    {
        for (auto& c : scene->models)
        {
            if (c.ParentID == m.modelId)
            {
                c.isChild = true;
                m.childrenID.push_back(c.modelId);
            }
        }
    }

    // Actualizar transformaciones
    for (auto& m : scene->models) m.UpdateTransform();

    scene->BuildOctree();
    return true;
}
