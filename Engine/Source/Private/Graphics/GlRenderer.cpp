#include "GlRenderer.hpp"

#include "Blackbox.hpp"
#include "Core/Engine.hpp"
#include "Core/FileIO.hpp"
#include "glad/glad.h"

namespace blackbox::graphics
{
    GlRenderer::GlRenderer()
    {
        int success {};
        char infoLog[512];

        const std::string vertexShaderContents = ::Engine.FileIO().ReadFile("Shaders/basic.vert");
        const char* vertexShaderSource = vertexShaderContents.c_str();
        
        const std::string fragmentShaderContents = ::Engine.FileIO().ReadFile("Shaders/basic.frag");
        const char* fragmentShaderSource = fragmentShaderContents.c_str();

        // Create vertex shader
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);
        
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            LogEngine->Error("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n{}", infoLog);
        }

        // Create fragment shader
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            LogEngine->Error("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n{}", infoLog);
        }

        // Create shader program
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            LogEngine->Error("ERROR::SHADER::PROGRAM::LINKING_FAILED\n{}", infoLog);
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Create Triangle & Co.
        float vertices[]
        {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f,
        };

        // Create, bind, and pass data to Vertex Buffer Object
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Set vertex attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
        glBindVertexArray(0);    
    }
    
    GlRenderer::~GlRenderer()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    };

    void GlRenderer::Render()
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}
