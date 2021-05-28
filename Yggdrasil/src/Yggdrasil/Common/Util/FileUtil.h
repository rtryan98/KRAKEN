// Copyright 2021 Robert Ryan. See LICENCE.md.

#pragma once
#include <string>
#include <vector>

namespace Ygg::FileUtil
{
    std::string ReadFileAsString(const std::string& filepath);

    const char* FileExtension(const std::string& filepath);

    bool ReadFileAsBytecode(const std::string& filepath, std::vector<uint32_t>& result);
}
