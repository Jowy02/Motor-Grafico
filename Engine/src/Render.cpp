#include "Application.h"
#include "Window.h"
#include "Render.h"
#include "Texture.h"

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

struct MeshData
{
    GLuint VAO = 0, VBO = 0, EBO = 0;
    bool initialized = false;
};

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

    //float vertices[] = {
    //     0.5f,  0.5f, 0.0f,  // top right
    //     0.5f, -0.5f, 0.0f,  // bottom right
    //    -0.5f, -0.5f, 0.0f,  // bottom left
    //    -0.5f,  0.5f, 0.0f,   // top left 

    //     0.5f,  0.5f, 0.0f,  // top right
    //     0.5f, -0.5f, 0.0f,  // bottom right
    //    -0.5f, -0.5f, 0.0f,  // bottom left
    //    -0.5f,  0.5f, 0.0f   // top left 
    //};
    //unsigned int indices[] = {  // note that we start from 0!
    //        0,1,2, 2,3,0,   // 36 of indices
    //        0,3,4, 4,5,0,
    //        0,5,6, 6,1,0,
    //        1,6,7, 7,2,1,
    //        7,4,3, 3,2,7,
    //        4,7,6, 6,5,4
    //};

    temp = Application::GetInstance().window.get()->window;

    return true;
}

// Called before the first frame
bool Render::Start()
{
    glEnable(GL_DEPTH_TEST);

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

	return true;
}

//bool Render::DrawTriangle() 
//{
//    glBindVertexArray(VAO);
//    glDrawArrays(GL_TRIANGLES, 0, 3);
//    return true;
//}
//bool Render::DrawElements() 
//{
//    glBindVertexArray(VAO);
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//    glBindVertexArray(0);
//
//    return true;
//}

bool Render::Draw3D(const GLfloat* vertices, size_t vertexCount, const GLuint* indices, size_t indexCount, float rotation, Texture* texture)
{
    //Application::GetInstance().camera.get()->Inputs(temp);
    Application::GetInstance().camera.get()->Matrix(45.0f, 0.1f, 100.0f, shaderProgram);

    static MeshData mesh;

    if (!mesh.initialized)
    {
        mesh.initialized = true;

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
    }

    // Usa tu shader
    glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
    
        // Assigns different transformations to each matrix
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

        // Enviar solo el modelo (la cámara ya envió view y projection)
        int modelLoc = glGetUniformLocation(shaderProgram, "model_matrix");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Decidir si usar textura
        GLint useTexLoc = glGetUniformLocation(shaderProgram, "useTexture");
        if (texture)
        {
            texture->texUnit(shaderProgram, "tex0", 0);
            texture->Bind();
            glUniform1i(useTexLoc, 1);
        }
        else
        {
            glUniform1i(useTexLoc, 0);
        }

        // Outputs the matrices into the Vertex Shader
    
        // Binds texture so that is appears in rendering
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    

    return true;
}

bool Render::PostUpdate()
{
    SDL_GL_SwapWindow(temp);
    return true;
}

// Called before quitting
bool Render::CleanUp()
{
  /*  glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);*/

    return true;
}