#include "GlShader.hpp"

#include "Blackbox.hpp"
#include "Core/Engine.hpp"
#include "Core/FileIO.hpp"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

namespace blackbox::graphics
{
    GlShader::GlShader(const std::string& vertexPath, const std::string& fragmentPath)
    {
        int success {};
        char infoLog[512];

        const std::string vertexShaderContents = ::Engine.FileIO().ReadFile(vertexPath);
        const char* vertexShaderSource = vertexShaderContents.c_str();
        
        const std::string fragmentShaderContents = ::Engine.FileIO().ReadFile(fragmentPath);
        const char* fragmentShaderSource = fragmentShaderContents.c_str();

        // Create vertex shader
        const unsigned vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);
        
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            LogEngine->Error("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n{}", infoLog);
        }

        // Create fragment shader
        const unsigned fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
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
    }

    void GlShader::Use() const
    {
        glUseProgram(shaderProgram);
    }

    void GlShader::SetBool(const std::string& name, const bool value) const
    {
        glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), static_cast<int>(value));
    }

    void GlShader::SetInt(const std::string& name, const int value) const
    {
        glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
    }

    void GlShader::SetFloat(const std::string& name, const float value) const
    {
        glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
    }

    void GlShader::SetFloat3(const std::string& name, const float3 value) const
    {
        glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), value.x, value.y, value.z);
    }

    void GlShader::SetMat4(const std::string& name, glm::mat4 value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
}
