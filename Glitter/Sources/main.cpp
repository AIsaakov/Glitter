// Local Headers
#include "glitter.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>

void renderObjects()
{
    // Background Fill Color
    glClearColor(0.5f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices for first triangle
    float vertices[] =
    {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    // Generate VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // Generate VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO); // Create a buffer and get its ID

    // Bind VAO, then VBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind the buffer to the GL_ARRAY_BUFFER target
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Upload vertex data to the buffer

    // Set vertex attribute pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void *>(0));
    glEnableVertexAttribArray(0);

    // Unbind VAO so other VAO calls won't unintentionally modify this VAO
    glBindVertexArray(0);

    // Source code for vertex shader
    const char * vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;

        void main()
        {
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    )";

    // Create vertex shader, set source code, and compile
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Check for compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Error compiling vertex shader:\n" << infoLog << std::endl;
    }

    // Create fragment shader, set source code, and compile
    const char * fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        }
    )";

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Check for compilation errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Error compiling fragment shader:\n" << infoLog << std::endl;
    }

    // Create shader program and ttach previously compiled vertex and fragment shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Error linking shader program:\n" << infoLog << std::endl;
    }

    // Rebind VAO
    glBindVertexArray(VAO);

    // Set shader program
    glUseProgram(shaderProgram);

    // Finally draw our triangle
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main(int argc, char * argv[])
{
    // Load GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a Window
    auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);

    // Check for Valid Context
    if (mWindow == nullptr) {
        std::cerr << "Failed to Create OpenGL Context" << std::endl;
        return EXIT_FAILURE;
    }

    // Create Context
    glfwMakeContextCurrent(mWindow);

    // Load OpenGL Functions
    gladLoadGL();
    std::cerr << "OpenGL " << glGetString(GL_VERSION) << std::endl;

    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false)
    {
        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(mWindow, true);
        }

        renderObjects();

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
