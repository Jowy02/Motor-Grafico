#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Texture.h"
#include "Scene.h"
#include "Model.h"

#define VSYNC true

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

SDL_Event event;

Render::Render() : Module()
{
	name = "render";
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
Render::~Render()
{
}
// Called before render is available
bool Render::Awake()
{
    //Triangle
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

    temp = Application::GetInstance().window.get()->window;

    return true;
}

// Called before the first frame
bool Render::Start()
{
    glEnable(GL_DEPTH_TEST);
    CreateGrid(10,20);
	return true;
}

// Called each loop iteration
bool Render::PreUpdate()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

	return true;
}

bool Render::Update(float dt)
{
    //Modelo se vea desde la posicón de la cámara correcta
    Application::GetInstance().camera.get()->Inputs(Application::GetInstance().window.get()->window);
    Application::GetInstance().camera.get()->Matrix(45.0f, 0.1f, 100.0f, shaderProgram);

	return true;
}

gemotryMesh Render::Draw3D(const GLfloat* vertices, size_t vertexCount, const GLuint* indices, size_t indexCount, float rotation, Texture* texture)
{
    //Application::GetInstance().camera.get()->Inputs(temp);
    Application::GetInstance().camera.get()->Matrix(45.0f, 0.1f, 100.0f, shaderProgram);

    static gemotryMesh mesh;
    mesh.indexCount = indexCount;

    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    // Posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Coordenadas de textura
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    return mesh;
}
void Render::CreateSphere()
{
    gemotryMesh mesh;

    float radius = 1.0f;
    unsigned int sectorCount = 32.0f;
    unsigned int stackCount = 16.0f;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const float PI = 3.14159265359f;
    float x, y, z, xy;                              // posición
    float nx, ny, nz, lengthInv = 1.0f / radius;   // normales
    float s, t;                                    // coordenadas de textura

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    // Generar vértices
    for (unsigned int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // empieza desde pi/2 hasta -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        for (unsigned int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // empieza desde 0 hasta 2pi

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);

            // Posición
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Color, aquí lo hacemos con un degradado simple
            vertices.push_back((x / radius + 1.0f) / 2.0f);
            vertices.push_back((y / radius + 1.0f) / 2.0f);
            vertices.push_back((z / radius + 1.0f) / 2.0f);

            // UVs
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            vertices.push_back(s);
            vertices.push_back(t);
        }
    }

    // Generar índices
    unsigned int k1, k2;
    for (unsigned int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    mesh.indexCount = indices.size();

    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Coordenadas de textura
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    Model model("NULL");
    model.Mmesh.EBO = mesh.EBO;
    model.Mmesh.VBO = mesh.VBO;
    model.Mmesh.VAO = mesh.VAO;
    model.Mmesh.indexCount = mesh.indexCount;

    model.name = "Sphere";

    Application::GetInstance().scene.get()->models.push_back(model);
}
gemotryMesh Render::CreateGrid(int size, int divisions)
{
    gemotryMesh mesh;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    float half = size / 2.0f;
    float step = (float)size / divisions;

    // Ejes X y Z (plano Y=0)
    for (int i = 0; i <= divisions; ++i)
    {
        float pos = -half + i * step;

        // Línea paralela al eje X
        vertices.insert(vertices.end(), { -half, 0.0f, pos,  0.5f, 0.5f, 0.5f, 0.0f, 0.0f });
        vertices.insert(vertices.end(), { half, 0.0f, pos,  0.5f, 0.5f, 0.5f, 1.0f, 0.0f });

        // Línea paralela al eje Z
        vertices.insert(vertices.end(), { pos, 0.0f, -half,  0.5f, 0.5f, 0.5f, 0.0f, 1.0f });
        vertices.insert(vertices.end(), { pos, 0.0f,  half,  0.5f, 0.5f, 0.5f, 1.0f, 1.0f });
    }

    // Generar índices secuenciales (2 por línea)
    for (unsigned int i = 0; i < vertices.size() / 8; ++i)
        indices.push_back(i);

    // === Buffers OpenGL ===
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Coordenadas de textura
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

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
void Render::CreateTriangle() 
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
        0, 1, 2,
        0, 2, 3,
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
    model.name = "Triangle";

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

    // Indices for vertices order
    GLuint indices2[] =
    {
        // Cara trasera
        0, 1, 2, 2, 3, 0,
        // Cara delantera
        4, 5, 6, 6, 7, 4,
        // Cara izquierda
        0, 4, 7, 7, 3, 0,
        // Cara derecha
        1, 5, 6, 6, 2, 1,
        // Cara inferior
        0, 1, 5, 5, 4, 0,
        // Cara superior
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

    Application::GetInstance().scene.get()->models.push_back(model);
}

void Render::CreateDiamond()
{
    GLfloat vertices2[] =
    { //     COORDINATES     /        COLORS      /   TexCoord  //
         0.0f,  0.5f,  0.0f,     1.0f, 0.8f, 0.2f, 0.5f, 1.0f, // top
         0.5f,  0.0f,  0.0f,     1.0f, 0.0f, 0.0f, 1.0f, 0.5f,
         0.0f,  0.0f,  0.5f,     0.0f, 1.0f, 0.0f, 0.5f, 0.0f,
        -0.5f,  0.0f,  0.0f,     0.0f, 0.0f, 1.0f, 0.0f, 0.5f,
         0.0f,  0.0f, -0.5f,     1.0f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.0f, -0.5f,  0.0f,     0.3f, 0.3f, 0.3f, 0.5f, 0.5f  // bottom
    };

    // Indices for vertices order
    GLuint indices2[] =
    {
        // Mitad superior
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 1,
        // Mitad inferior
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

    Application::GetInstance().scene.get()->models.push_back(model);
}

bool Render::PostUpdate()
{
    SDL_GL_SwapWindow(temp);
    return true;
}

// Called before quitting
bool Render::CleanUp()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    return true;
}