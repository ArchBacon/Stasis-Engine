#include "Core/FileIO.hpp"

#include <fstream>

#include "Blackbox.hpp"

namespace blackbox
{
    FileIO::FileIO() = default;
    FileIO::~FileIO() = default;

    std::string FileIO::ReadFile(const std::string& filepath)
    {
        std::ifstream file(filepath, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            LogEngine->Error("Could not open file: {}", filepath);
        }

        const std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::string content(size, '\0'); // Zero-fill content
        file.read(content.data(), size);

        return content;
    }
}
