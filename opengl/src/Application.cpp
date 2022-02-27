#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#define STBI_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource; 
};
static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << "\n";
        }
    }
    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    GLCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    // error handling
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*) alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    GLCall(unsigned int program = glCreateProgram());
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));
    return program;
}
int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 960, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "glew init failed" << std::endl;
    }
    std::cout << glGetString(GL_VERSION) << std::endl; 
    {
        float vertices[] = {
            // positions          colors              texture
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
             0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right 
             0.5f,  0.5f, 0.0f,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f, // top right
            -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f, // top left

            -0.5f, -0.5f, 1.0f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
             0.5f, -0.5f, 1.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right 
             0.5f,  0.5f, 1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // top right
            -0.5f,  0.5f, 1.0f,   0.0f, 0.0f, 0.0f,   0.0f, 1.0f, // top left
        };

        unsigned int indices[] = {
            0, 1, 2, 
            2, 3, 0,

            1, 2, 5,
            5, 6, 2,

            0, 1, 4,
            1, 4, 5,

            4, 5, 6,
            4, 6, 7,

            0, 3, 4,
            3, 4, 7,

            3, 2, 7,
            2, 7, 6,
        };

        VertexArray va;
        VertexBuffer vb(vertices, 8 * 8 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(3);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        //GLCall(glEnableVertexAttribArray(0));
        //GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
        //GLCall(glEnableVertexAttribArray(1));
        //GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));
        //GLCall(glEnableVertexAttribArray(2));
        //GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))));

        IndexBuffer ib(indices, 6 * 6);

        ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
        //std::cout << "Vertex Source" << std::endl;
        //std::cout << source.VertexSource << std::endl;
        //std::cout << "Fragment Source" << std::endl;
        //std::cout << source.FragmentSource << std::endl;

        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        GLCall(int scale = glGetUniformLocation(shader, "scale"));
        GLCall(int u_MVP = glGetUniformLocation(shader, "u_MVP"));
        GLCall(int u_model = glGetUniformLocation(shader, "model"));
        GLCall(int u_view = glGetUniformLocation(shader, "view"));
        GLCall(glUseProgram(shader));

        unsigned int texture;
        GLCall(glGenTextures(1, &texture));
        GLCall(glBindTexture(GL_TEXTURE_2D, texture));
        // set the texture wrapping/filtering options (on the currently bound texture object)
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        // load and generate the texture
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load("res/textures/cat2.jpg", &width, &height, &nrChannels, 0);
        if (data)
        {
            GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
            GLCall(glGenerateMipmap(GL_TEXTURE_2D));
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        /* Loop until the user closes the window */
        GLCall(glEnable(GL_DEPTH_TEST));
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            float timeValue = glfwGetTime();
            float redValue = 0.2f * cos(3 * timeValue) + 0.8f;
            float greenValue = 0.5f * sin(3 * timeValue) + 0.5f;
            float blueValue = 0.5f * sin(6 * timeValue) + 0.5f;
            GLCall(glUniform1f(scale, 1.0f));

            glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, timeValue, glm::vec3(1.0f, 1.0f, 1.0f)); 
            glm::mat4 view = glm::mat4(1.0f);
            // note that we're translating the scene in the reverse direction of where we want to move
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));

            GLCall(glUniformMatrix4fv(u_MVP, 1, GL_FALSE, &proj[0][0]));
            GLCall(glUniformMatrix4fv(u_model, 1, GL_FALSE, &model[0][0]));
            GLCall(glUniformMatrix4fv(u_view, 1, GL_FALSE, &view[0][0]));
            GLCall(glDrawElements(GL_TRIANGLES, 6*6, GL_UNSIGNED_INT, nullptr));

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

        GLCall(glDeleteProgram(shader));
    }
    glfwTerminate();
    return 0;
}