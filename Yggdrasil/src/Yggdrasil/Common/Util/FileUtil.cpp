// Copyright 2021 Robert Ryan. See LICENCE.md.

#include "Yggdrasil/pch.h"
#include "Yggdrasil/Common/Util/FileUtil.h"

#include <fstream>
#include <sstream>

namespace Ygg::FileUtil
{
    std::string ReadFileAsString(const std::string& filepath)
    {
        std::stringstream result{};
        std::ifstream input{};
        input.open(filepath);
        if (input.fail())
        {
            YGG_WARN("Failed to open file: {0}", filepath.c_str());
            input.close();
            return result.str();
        }
        std::string line{};
        while (getline(input, line))
        {
            result << line << '\n';
        }
        input.close();
        return result.str();
    }

    const char* FileExtension(const std::string& filepath)
    {
        const uint32_t dot{ static_cast<uint32_t>(filepath.find('.')) };
        return (dot == std::string::npos)
            ? ""
            : filepath.substr(filepath.rfind('.') + 1).c_str();
    }

    bool ReadFileAsBytecode(const std::string& filepath, std::vector<uint32_t>& result)
    {
        std::ifstream input{ filepath, std::ios::ate | std::ios::binary };
        if (!input.is_open())
        {
            return false;
        }

        auto size{ input.tellg() };
        result.resize(
            (size % sizeof(uint8_t) == 0)
            ? (size / sizeof(uint8_t))
            : (size / sizeof(uint8_t) + 1)
            );
        input.seekg(0);
        input.read(reinterpret_cast<char*>(result.data()), size);
        return true;
    }
}
