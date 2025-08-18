﻿#pragma once

#include <string>

#include "Core/Types.hpp"

namespace blackbox
{
    class FileIO;
}

namespace blackbox::graphics
{
    class GlShader
    {
        unsigned shaderProgram {0};
        FileIO& fileIO;
        
    public:
        GlShader(FileIO& fileIO, const std::string& vertexPath, const std::string& fragmentPath);

        void Use() const;

        void SetBool(const std::string& name, bool value) const;
        void SetInt(const std::string& name, int value) const;
        void SetFloat(const std::string& name, float value) const;
        void SetFloat3(const std::string& name, float3 value) const;
        void SetMat4(const std::string& name, glm::mat4 value) const;
    };
}
