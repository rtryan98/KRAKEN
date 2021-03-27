#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/util/FileUtil.h"
#include "Yggdrasil/core/util/Log.h"
#include "Yggdrasil/Defines.h"

#include <string>
#include <fstream>
#include <iostream>

namespace ygg::fileutil
{
    std::string readStringFromFile(const std::string& filename)
    {
        std::string result{};
        std::ifstream input{};
        input.open(filename);
        if (input.fail())
        {
            YGGDRASIL_CORE_WARN("Failed to open file: {0}", filename.c_str());
            input.close();
            return result;
        }
        std::string line{};
        while (getline(input, line))
        {
            result.append(line + '\n');
        }
        input.close();
        return result;
    }
}
