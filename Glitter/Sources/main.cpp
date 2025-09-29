// Local Headers
#include "glitter.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>

void renderObjects(unsigned int VAO, unsigned int shaderProgram)
{
    // Rebind VAO
    glBindVertexArray(VAO);

    // Set shader program
    glUseProgram(shaderProgram);

    // Finally draw our 2 triangles (square) using the values set in the EBO, which indexes into the VBO
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Unbind VAO so other VAO calls won't unintentionally modify this VAO
    glBindVertexArray(0);
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

    // Background Fill Color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices for triangle
    float vertices[] = {
        // positions        // colors
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top
    };
    unsigned int indices[] =
    {
        // Note that we start from 0!
        0, 1, 2   // first triangle
    };

    // Generate VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // Generate VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // Generate EBO
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind and set VBO data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind and set EBO data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set vertex attribute pointer for position
    constexpr GLuint positionAttribLocation = 0;
    constexpr GLuint positionStride = 6 * sizeof(float);  // Each vertex has 6 floats (3 for position, 3 for color)
    constexpr GLuint positionOffset = 0;  // Start reading from the beginning of the buffer
    glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, positionStride, reinterpret_cast<void*>(positionOffset));
    glEnableVertexAttribArray(positionAttribLocation);

    // Set vertex attribute pointer for color
    constexpr GLuint colorAttribLocation = 1;
    constexpr GLuint colorStride = 6 * sizeof(float);  // Each vertex has 6 floats (3 for position, 3 for color)
    constexpr GLuint colorOffset = 3 * sizeof(float);  // Start reading from after position values
    glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, colorStride, reinterpret_cast<void*>(colorOffset));
    glEnableVertexAttribArray(colorAttribLocation);

    // Unbind VAO so other VAO calls won't unintentionally modify this VAO
    glBindVertexArray(0);

    // Source code for vertex shader
    const char * vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;

        out vec3 ourColor; // output a color to the fragment shader

        void main()
        {
            gl_Position = vec4(aPos, 1.0);
            ourColor = aColor;
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
        in vec3 ourColor;

        void main()
        {
            FragColor = vec4(ourColor, 1.0);
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

    // Create shader program and attach previously compiled vertex and fragment shaders
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

    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false)
    {
        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(mWindow, true);
        }

        // Assign
        float timeValue = static_cast<float>(glfwGetTime());
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        // Note that finding the uniform location does not require you to use the shader program first,
        // but updating a uniform does require you to first use the program (by calling glUseProgram),
        // because it sets the uniform on the currently active shader program.
        glUseProgram(shaderProgram);
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f); // (location, r, g, b, a)

        renderObjects(VAO, shaderProgram);

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
    glfwTerminate();
    return EXIT_SUCCESS;
}
