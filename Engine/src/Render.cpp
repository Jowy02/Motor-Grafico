#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Texture.h"
#include "Scene.h"
#include "Model.h"
#include <iostream>
#include "Menus.h"

#define VSYNC true

// --- SHADERS ---
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aTexCoord;\n"

"out vec3 ourColor;\n"
"out vec2 TexCoord;\n"

"uniform mat4 model_matrix;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"

"void main()\n"
"{\n"
"   gl_Position = projection * view * model_matrix * vec4(aPos, 1.0f);\n"
"   ourColor = aColor;\n"
"   TexCoord = aTexCoord;\n"

"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"
"out vec4 FragColor;\n"
"uniform sampler2D tex0;\n"
"uniform bool useTexture;\n" 
"void main()\n"
"{\n"
"   if(useTexture)\n"
"       FragColor = texture(tex0, TexCoord);\n"
"   else\n"
"       FragColor = vec4(ourColor, 1.0);\n"
"}\n\0";

const char* normalVertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model_matrix;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model_matrix * vec4(aPos, 1.0);\n"
"}\0";

const char* normalFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n" // Verde
"}\0";

// --- CONSTRUCTOR ---
Render::Render() : Module()
{
	name = "Render";
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
Render::~Render()
{
}

// --- LIFE CYCLE ---
bool Render::Awake()
{
    // Triangle
    bool ret = true;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint normalVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(normalVertexShader, 1, &normalVertexShaderSource, NULL);
    glCompileShader(normalVertexShader);

    GLuint normalFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(normalFragmentShader, 1, &normalFragmentShaderSource, NULL);
    glCompileShader(normalFragmentShader);

    normalShaderProgram = glCreateProgram();
    glAttachShader(normalShaderProgram, normalVertexShader);
    glAttachShader(normalShaderProgram, normalFragmentShader);
    glLinkProgram(normalShaderProgram);

    glDeleteShader(normalVertexShader);
    glDeleteShader(normalFragmentShader);

    temp = Application::GetInstance().window.get()->window;

    return true;
}

bool Render::Start()
{
    glEnable(GL_DEPTH_TEST);
    CreateGrid(10,20);
	return true;
}

bool Render::PreUpdate()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

	return true;
}

bool Render::Update(float dt)
{
    // Model is seen from the correct camera position
    Application::GetInstance().camera.get()->Inputs(Application::GetInstance().window.get()->window);
    Application::GetInstance().camera.get()->Matrix(45.0f, 0.1f, 100.0f, shaderProgram);

    ShowFaceNormals();
    ShowVertexNormals();

    return true;
}

// ---- MESH CREATION ---
void Render::CreatePyramid()
{
    GLfloat vertices2[] =
    { //     COORDINATES     /        COLORS      /   TexCoord  //
        -0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
        -0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
         0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
         0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
         0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f

    };

    // Indices for vertices order
    GLuint indices2[] =
    {
        0, 3, 2,
        0, 2, 1,
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };


    int vertexCount = sizeof(vertices2) / sizeof(float);
    int indexCount = sizeof(indices2) / sizeof(unsigned int);
  
    Model model("NULL");

    static gemotryMesh mesh = Application::GetInstance().render.get()->Draw3D(vertices2, vertexCount, indices2, indexCount, 60.0f);

    model.Mmesh.VAO = mesh.VAO;
    model.Mmesh.EBO = mesh.EBO;
    model.Mmesh.VBO = mesh.VBO;
    model.Mmesh.indexCount = mesh.indexCount;
    model.Mmesh.texture = mesh.texture;
    model.name = "Pyramid";

    float baseSize = 1.0f;
    float height = 0.8f;
    float half = baseSize / 2.0f;

    GLfloat vertices[] = {
        -half, 0.0f,  half,   // frente izq
        -half, 0.0f, -half,   // atrás izq
         half, 0.0f, -half,   // atrás der
         half, 0.0f,  half,   // frente der
         0.0f,  height, 0.0f  // arriba
    };
    auto vertexNormals = CalculateVertexNormalsPrueva(vertices2, indices2, vertexCount, indexCount, 30.0f);
    auto tangentData = CalculateTangentsAndBitangents(vertices2, indices2, vertexNormals, vertexCount, indexCount);
    std::vector<glm::vec3> tangents = tangentData.first;
    std::vector<glm::vec3> bitangents = tangentData.second;

   model.Normalmesh = DrawFaceNormals(vertices2, indices2, indexCount, model.normalLines);
   model.VertexNormalmesh = DrawVertexNormalsFromMesh(vertices2, vertexCount, vertexNormals, tangents, bitangents, model.vertexNormalLines);



    Application::GetInstance().scene.get()->models.push_back(model);
}


void Render::CreateCube()
{
    GLfloat vertices2[] =
    { //     COORDINATES     /        COLORS      /   TexCoord  //
       -0.5f, -0.5f, -0.5f,    0.80f, 0.40f, 0.0f,    0.0f, 0.0f, // 0
        0.5f, -0.5f, -0.5f,    0.80f, 0.40f, 0.0f,    1.0f, 0.0f, // 1
        0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,      1.0f, 1.0f, // 2
       -0.5f,  0.5f, -0.5f,    0.80f, 0.40f, 0.0f,    0.0f, 1.0f, // 3
       -0.5f, -0.5f,  0.5f,    0.80f, 0.40f, 0.0f,    0.0f, 0.0f, // 4
        0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,      1.0f, 0.0f, // 5
        0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,      1.0f, 1.0f, // 6
       -0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,      0.0f, 1.0f  // 7
    };

    // Indices for vertex order
    GLuint indices2[] =
    {
        // Back face
        0, 1, 2, 2, 3, 0,
        // Front face
        4, 5, 6, 6, 7, 4,
        // Left face
        0, 4, 7, 7, 3, 0,
        // Right face
        1, 5, 6, 6, 2, 1,
        // Bottom face
        0, 1, 5, 5, 4, 0,
        // Top face
        3, 2, 6, 6, 7, 3
    };

    int vertexCount = sizeof(vertices2) / sizeof(float);
    int indexCount = sizeof(indices2) / sizeof(unsigned int);

    Model model("NULL");

    static gemotryMesh mesh = Application::GetInstance().render.get()->Draw3D(vertices2, vertexCount, indices2, indexCount, 60.0f);

    model.Mmesh.VAO = mesh.VAO;
    model.Mmesh.EBO = mesh.EBO;
    model.Mmesh.VBO = mesh.VBO;
    model.Mmesh.indexCount = mesh.indexCount;
    model.Mmesh.texture = mesh.texture;
    model.name = "Cube";


    auto vertexNormals = CalculateVertexNormalsPrueva(vertices2, indices2, vertexCount, indexCount, 30.0f);
    auto tangentData = CalculateTangentsAndBitangents(vertices2, indices2, vertexNormals, vertexCount, indexCount);
    std::vector<glm::vec3> tangents = tangentData.first;
    std::vector<glm::vec3> bitangents = tangentData.second;

    model.Normalmesh = DrawFaceNormals(vertices2, indices2, indexCount, model.normalLines);
    model.VertexNormalmesh = DrawVertexNormalsFromMesh(vertices2, vertexCount, vertexNormals, tangents, bitangents, model.vertexNormalLines);


    Application::GetInstance().scene.get()->models.push_back(model);
}

void Render::CreateDiamond()
{
    GLfloat vertices2[] =
    { //     COORDINATES     /        COLORS      /    TexCoord  //
         0.0f,  0.5f,  0.0f,     1.0f, 0.8f, 0.2f,    0.5f, 1.0f, // TOP
         0.5f,  0.0f,  0.0f,     1.0f, 0.0f, 0.0f,    1.0f, 0.5f,
         0.0f,  0.0f,  0.5f,     0.0f, 1.0f, 0.0f,    0.5f, 0.0f,
        -0.5f,  0.0f,  0.0f,     0.0f, 0.0f, 1.0f,    0.0f, 0.5f,
         0.0f,  0.0f, -0.5f,     1.0f, 0.5f, 0.5f,    0.5f, 0.5f,
         0.0f, -0.5f,  0.0f,     0.3f, 0.3f, 0.3f,    0.5f, 0.5f  // BOTTOM
    };

    // Indices for vertex order
    GLuint indices2[] =
    {
        // Upper half
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 1,
        // Lower half
        5, 2, 1,
        5, 3, 2,
        5, 4, 3,
        5, 1, 4
    };

    int vertexCount = sizeof(vertices2) / sizeof(float);
    int indexCount = sizeof(indices2) / sizeof(unsigned int);

    Model model("NULL");
    static gemotryMesh mesh = Application::GetInstance().render.get()->Draw3D(vertices2, vertexCount, indices2, indexCount, 60.0f);

    model.Mmesh.VAO = mesh.VAO;
    model.Mmesh.EBO = mesh.EBO;
    model.Mmesh.VBO = mesh.VBO;
    model.Mmesh.indexCount = mesh.indexCount;
    model.Mmesh.texture = mesh.texture;
    model.name = "Diamond";

    auto vertexNormals = CalculateVertexNormalsPrueva(vertices2, indices2, vertexCount, indexCount, 30.0f);
    auto tangentData = CalculateTangentsAndBitangents(vertices2, indices2, vertexNormals, vertexCount, indexCount);
    std::vector<glm::vec3> tangents = tangentData.first;
    std::vector<glm::vec3> bitangents = tangentData.second;

    model.Normalmesh = DrawFaceNormals(vertices2, indices2, indexCount, model.normalLines);
 
    model.VertexNormalmesh = DrawVertexNormalsFromMesh(vertices2, vertexCount, vertexNormals, tangents, bitangents, model.vertexNormalLines);

    Application::GetInstance().scene.get()->models.push_back(model);
}

void Render::CreateSphere()
{
    gemotryMesh mesh;

    float radius = 1.0f;
    unsigned int sectorCount = 32;  // Number of longitudinal slices
    unsigned int stackCount = 16;   // Number of latitudinal slices

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const float PI = 3.14159265359f;
    float x, y, z, xy;                                  // Position
    float nx, ny, nz, lengthInv = 1.0f / radius;        // Normals
    float s, t;                                         // Texture Coordinates

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    // Generate vertices
    for (unsigned int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for (unsigned int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);

            // Position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Color, using a simple gradient based on position
            vertices.push_back((x / radius + 1.0f) / 2.0f);
            vertices.push_back((y / radius + 1.0f) / 2.0f);
            vertices.push_back((z / radius + 1.0f) / 2.0f);

            // Texture coordinates (UV)
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            vertices.push_back(s);
            vertices.push_back(t);
        }
    }

    // Generate indices for the sphere’s triangles
    unsigned int k1, k2;
    for (unsigned int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            if (i != 0)                 // Upper triangle
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1))  // Lower triangle
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2 + 1);
                indices.push_back(k2);
            }
        }
    }

    mesh.indexCount = indices.size();

    // Upload vertex and index data to GPU buffers
    mesh = SetBuffers(mesh, vertices, indices);

    // Create a model and assign mesh data
    Model model("NULL");
    model.Mmesh.EBO = mesh.EBO;
    model.Mmesh.VBO = mesh.VBO;
    model.Mmesh.VAO = mesh.VAO;
    model.Mmesh.indexCount = mesh.indexCount;

    model.name = "Sphere";



    auto vertexNormals = CalculateVertexNormalsPrueva(vertices.data(), indices.data(), vertices.size(), indices.size(), 30.0f);
    auto tangentData = CalculateTangentsAndBitangents(vertices.data(), indices.data(), vertexNormals, vertices.size(), indices.size());
    std::vector<glm::vec3> tangents = tangentData.first;
    std::vector<glm::vec3> bitangents = tangentData.second;

    model.Normalmesh = DrawFaceNormals(vertices.data(), indices.data(), indices.size(), model.normalLines);
    model.VertexNormalmesh = DrawVertexNormalsFromMesh(vertices.data(), vertices.size(), vertexNormals, tangents, bitangents, model.vertexNormalLines);

    Application::GetInstance().scene.get()->models.push_back(model);
}

gemotryMesh Render::CreateGrid(int size, int divisions)
{
    gemotryMesh mesh;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    float half = size / 2.0f;
    float step = (float)size / divisions;

    for (int i = 0; i <= divisions; ++i)
    {
        float pos = -half + i * step;

        // Line parallel to X axis
        vertices.insert(vertices.end(), { -half, 0.0f, pos,  0.5f, 0.5f, 0.5f, 0.0f, 0.0f });
        vertices.insert(vertices.end(), { half, 0.0f, pos,  0.5f, 0.5f, 0.5f, 1.0f, 0.0f });

        // Line parallel to Z axis
        vertices.insert(vertices.end(), { pos, 0.0f, -half,  0.5f, 0.5f, 0.5f, 0.0f, 1.0f });
        vertices.insert(vertices.end(), { pos, 0.0f,  half,  0.5f, 0.5f, 0.5f, 1.0f, 1.0f });
    }

    for (unsigned int i = 0; i < vertices.size() / 8; ++i)
        indices.push_back(i);

    mesh = SetBuffers(mesh, vertices, indices);
    mesh.indexCount = indices.size();

    Model model("NULL");
    model.Mmesh.VAO = mesh.VAO;
    model.Mmesh.EBO = mesh.EBO;
    model.Mmesh.VBO = mesh.VBO;
    model.Mmesh.indexCount = mesh.indexCount;
    model.Mmesh.texture = mesh.texture;
    model.name = "Grid";

    Application::GetInstance().scene.get()->models.push_back(model);

    return mesh;
}

gemotryMesh Render::SetBuffers(gemotryMesh Mesh, std::vector<float> vertices, std::vector<unsigned int> indices)
{
    // === OpenGL Buffers ===
    glGenVertexArrays(1, &Mesh.VAO);
    glGenBuffers(1, &Mesh.VBO);
    glGenBuffers(1, &Mesh.EBO);

    glBindVertexArray(Mesh.VAO);

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, Mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // === Vertex attributes ===
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinates (UV)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    return Mesh;
}

// --- NORMAL VISUALIZATION ---
void Render::ShowFaceNormals()
{
    if (FaceNormals)
    {
        //glDisable(GL_DEPTH_TEST);
        glUseProgram(normalShaderProgram);
        GLuint modelLoc = glGetUniformLocation(normalShaderProgram, "model_matrix");
        Application::GetInstance().camera.get()->Matrix(45.0f, 0.2f, 100.0f, normalShaderProgram);

        Model* selected = Application::GetInstance().menus.get()->selectedObj;
        if (selected && selected->Normalmesh.VAO != 0)
        {
            glm::mat4 modelMat = selected->GetModelMatrix();
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

            glBindVertexArray(selected->Normalmesh.VAO);
            glDrawArrays(GL_LINES, 0, selected->Normalmesh.indexCount);
            glBindVertexArray(0);
        }
    }
}

void Render::ShowVertexNormals()
{
    if (VertexNormals)
    {
        //glDisable(GL_DEPTH_TEST);
        glUseProgram(normalShaderProgram);
        GLuint modelLoc = glGetUniformLocation(normalShaderProgram, "model_matrix");
        Application::GetInstance().camera.get()->Matrix(45.0f, 0.2f, 100.0f, normalShaderProgram);

        Model* selected = Application::GetInstance().menus.get()->selectedObj;
        if (selected && selected->Normalmesh.VAO != 0)
        {
            glm::mat4 modelMat = selected->GetModelMatrix();
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

            glBindVertexArray(selected->VertexNormalmesh.VAO);
            glDrawArrays(GL_LINES, 0, selected->VertexNormalmesh.indexCount);
            glBindVertexArray(0);
        }
    }
}

// --- MESH DRAWING ---
gemotryMesh Render::Draw3D(const GLfloat* vertices, size_t vertexCount, const GLuint* indices, size_t indexCount, float rotation, Texture* texture)
{
    // Set the camera projection and view matrices
    //Application::GetInstance().camera.get()->Inputs(temp); // (commented out input handling)
    Application::GetInstance().camera.get()->Matrix(45.0f, 0.1f, 100.0f, shaderProgram);

    static gemotryMesh mesh;
    mesh.indexCount = indexCount;

    // Generate buffers
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float), vertices, GL_STATIC_DRAW);

    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // Vertex attributes

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinates (UV)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    return mesh;
}

gemotryMesh Render::DrawFaceNormals(const float* vertices, const unsigned int* indices, size_t indexCount, std::vector<float>& outLines)
{
    outLines.clear();
    gemotryMesh normalMesh;

    for (size_t i = 0; i < indexCount; i += 3)
    {
        int i0 = indices[i] * 8;
        int i1 = indices[i + 1] * 8;
        int i2 = indices[i + 2] * 8;

        float x0 = vertices[i0], y0 = vertices[i0 + 1], z0 = vertices[i0 + 2];
        float x1 = vertices[i1], y1 = vertices[i1 + 1], z1 = vertices[i1 + 2];
        float x2 = vertices[i2], y2 = vertices[i2 + 1], z2 = vertices[i2 + 2];

        float cx = (x0 + x1 + x2) / 3.0f;
        float cy = (y0 + y1 + y2) / 3.0f;
        float cz = (z0 + z1 + z2) / 3.0f;

        float ux = x2 - x0, uy = y2 - y0, uz = z2 - z0;
        float vx = x1 - x0, vy = y1 - y0, vz = z1 - z0;


        float nx = uy * vz - uz * vy;
        float ny = uz * vx - ux * vz;
        float nz = ux * vy - uy * vx;


        float length = sqrt(nx * nx + ny * ny + nz * nz);
        if (length != 0.0f)
        {
            nx /= length;
            ny /= length;
            nz /= length;
        }

        float scale = 0.1f;
        float ex = cx + nx * scale;
        float ey = cy + ny * scale;
        float ez = cz + nz * scale;

        // Add normal line
        outLines.push_back(cx); outLines.push_back(cy); outLines.push_back(cz);
        outLines.push_back(ex); outLines.push_back(ey); outLines.push_back(ez);
    }

    // Create VAO/VBO for normals
    glGenVertexArrays(1, &normalMesh.VAO);
    glGenBuffers(1, &normalMesh.VBO);

    glBindVertexArray(normalMesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, normalMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, outLines.size() * sizeof(float), outLines.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    normalMesh.indexCount = outLines.size() / 3;

    return normalMesh;
}

gemotryMesh Render::DrawVertexNormalsFromMesh(const float* vertices, size_t vertexCount, const std::vector<glm::vec3>& vertexNormals, const std::vector<glm::vec3>& tangents, const std::vector<glm::vec3>& bitangents, std::vector<float>& outLines)
{
    outLines.clear();
    gemotryMesh normalMesh;

    const bool hasTangents = !tangents.empty();
    const bool hasBitangents = !bitangents.empty();

    for (size_t i = 0; i < vertexCount; i += 8)
    {
        float x = vertices[i];
        float y = vertices[i + 1];
        float z = vertices[i + 2];

       /* float nx = vertices[i + 3];
        float ny = vertices[i + 4];
        float nz = vertices[i + 5];*/

        glm::vec3 normal = vertexNormals[i / 8];
        float nx = normal.x;
        float ny = normal.y;
        float nz = normal.z;

        float scale = 0.1f;

        // Normal (Green)
        outLines.push_back(x); outLines.push_back(y); outLines.push_back(z);
        outLines.push_back(x + nx * scale); outLines.push_back(y + ny * scale); outLines.push_back(z + nz * scale);

        // Tangent (Red)
        if (hasTangents)
        {
            const glm::vec3& t = tangents[i / 8];
            outLines.push_back(x); outLines.push_back(y); outLines.push_back(z);
            outLines.push_back(x + t.x * scale); outLines.push_back(y + t.y * scale); outLines.push_back(z + t.z * scale);
        }

        // Bitangent (Blue)
        if (hasBitangents)
        {
            const glm::vec3& b = bitangents[i / 8];
            outLines.push_back(x); outLines.push_back(y); outLines.push_back(z);
            outLines.push_back(x + b.x * scale); outLines.push_back(y + b.y * scale); outLines.push_back(z + b.z * scale);
        }
    }

    glGenVertexArrays(1, &normalMesh.VAO);
    glGenBuffers(1, &normalMesh.VBO);

    glBindVertexArray(normalMesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, normalMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, outLines.size() * sizeof(float), outLines.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    normalMesh.indexCount = outLines.size() / 3;
    return normalMesh;
}


// --- DEATH CYCLE ---
bool Render::PostUpdate()
{
    // Swap the window buffers (double buffering)
    SDL_GL_SwapWindow(temp);

    return true;
}

bool Render::CleanUp()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    return true;
}

std::vector<glm::vec3> Render::CalculateVertexNormalsPrueva(const GLfloat* vertices, const GLuint* indices, int vertexCount, int indexCount, float smoothingAngleDeg)
{
    int stride = 8; // cada vértice: x, y, z, r, g, b, u, v
    int numVertices = vertexCount / stride;

    std::vector<glm::vec3> vertexNormals(numVertices, glm::vec3(0.0f));

    // 1️⃣ Calcular las normales de cada cara
    std::vector<glm::vec3> faceNormals(indexCount / 3);
    for (int i = 0; i < indexCount; i += 3) {
        GLuint i0 = indices[i];
        GLuint i1 = indices[i + 1];
        GLuint i2 = indices[i + 2];

        glm::vec3 v0(vertices[i0 * stride + 0], vertices[i0 * stride + 1], vertices[i0 * stride + 2]);
        glm::vec3 v1(vertices[i1 * stride + 0], vertices[i1 * stride + 1], vertices[i1 * stride + 2]);
        glm::vec3 v2(vertices[i2 * stride + 0], vertices[i2 * stride + 1], vertices[i2 * stride + 2]);

        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        faceNormals[i / 3] = normal;
    }

    // 2️⃣ Convertir smoothing angle a radianes
    float smoothingAngle = glm::radians(smoothingAngleDeg);

    // 3️⃣ Calcular normales por vértice
    for (int v = 0; v < numVertices; ++v) {
        glm::vec3 currentVertex(vertices[v * stride + 0],
            vertices[v * stride + 1],
            vertices[v * stride + 2]);

        glm::vec3 accumulatedNormal(0.0f);
        int count = 0;

        // Recorrer todas las caras que usan este vértice
        for (int f = 0; f < indexCount; f += 3) {
            GLuint i0 = indices[f];
            GLuint i1 = indices[f + 1];
            GLuint i2 = indices[f + 2];

            if (i0 == v || i1 == v || i2 == v) {
                glm::vec3 faceN = faceNormals[f / 3];

                // Comparamos esta cara con todas las demás adyacentes
                bool include = true;
                for (int g = 0; g < indexCount; g += 3) {
                    if (g == f) continue;

                    GLuint j0 = indices[g];
                    GLuint j1 = indices[g + 1];
                    GLuint j2 = indices[g + 2];

                    // Si comparten este vértice
                    if (j0 == v || j1 == v || j2 == v) {
                        glm::vec3 otherN = faceNormals[g / 3];
                        float angle = acosf(glm::clamp(glm::dot(faceN, otherN), -1.0f, 1.0f));
                        if (angle > smoothingAngle) {
                            include = false;
                            break;
                        }
                    }
                }

                if (include) {
                    accumulatedNormal += faceN;
                    count++;
                }
            }
        }

        if (count > 0)
            vertexNormals[v] = glm::normalize(accumulatedNormal);
        else
            vertexNormals[v] = glm::vec3(0, 1, 0); // fallback
    }

    return vertexNormals;
}


std::pair<std::vector<glm::vec3>, std::vector<glm::vec3>> Render::CalculateTangentsAndBitangents(const GLfloat* vertices, const GLuint* indices, const std::vector<glm::vec3>& vertexNormals, int vertexCount, int indexCount)
{
      int stride = 8; // x, y, z, r, g, b, u, v
    int numVertices = vertexCount / stride;

    std::vector<glm::vec3> tangents(numVertices, glm::vec3(0.0f));
    std::vector<glm::vec3> bitangents(numVertices, glm::vec3(0.0f));

    // --- 1️⃣ Recorremos cada triángulo ---
    for (int i = 0; i < indexCount; i += 3)
    {
        GLuint i0 = indices[i];
        GLuint i1 = indices[i + 1];
        GLuint i2 = indices[i + 2];

        // posiciones
        glm::vec3 v0(vertices[i0 * stride + 0], vertices[i0 * stride + 1], vertices[i0 * stride + 2]);
        glm::vec3 v1(vertices[i1 * stride + 0], vertices[i1 * stride + 1], vertices[i1 * stride + 2]);
        glm::vec3 v2(vertices[i2 * stride + 0], vertices[i2 * stride + 1], vertices[i2 * stride + 2]);

        // UVs
        glm::vec2 uv0(vertices[i0 * stride + 6], vertices[i0 * stride + 7]);
        glm::vec2 uv1(vertices[i1 * stride + 6], vertices[i1 * stride + 7]);
        glm::vec2 uv2(vertices[i2 * stride + 6], vertices[i2 * stride + 7]);

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec2 deltaUV1 = uv1 - uv0;
        glm::vec2 deltaUV2 = uv2 - uv0;

        float denom = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        float f = (fabs(denom) > 1e-6f) ? (1.0f / denom) : 0.0f;

        glm::vec3 tangent(
            f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
            f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
            f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
        );

        glm::vec3 bitangent(
            f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
            f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
            f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z)
        );

        // --- 2️⃣ Acumular en los vértices ---
        tangents[i0] += tangent; tangents[i1] += tangent; tangents[i2] += tangent;
        bitangents[i0] += bitangent; bitangents[i1] += bitangent; bitangents[i2] += bitangent;
    }

    // --- 3️⃣ Ortonormalizar con las normales ---
    for (size_t i = 0; i < vertexNormals.size(); ++i)
    {
        glm::vec3 n = vertexNormals[i];
        glm::vec3 t = tangents[i];

        // proyectar tangente sobre el plano perpendicular a la normal
        t = glm::normalize(t - n * glm::dot(n, t));

        // recalcular bitangente ortogonal
        glm::vec3 b = glm::cross(n, t);

        // asegurar la misma orientación que la acumulada
        if (glm::dot(b, bitangents[i]) < 0.0f)
            b = -b;

        tangents[i] = glm::normalize(t);
        bitangents[i] = glm::normalize(b);
    }

    return { tangents, bitangents };
}

