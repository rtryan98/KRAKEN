# BSD 3-Clause License
# 
# Copyright (c) 2021, Robert Ryan
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

file(GLOB_RECURSE "YGG_ENGINE_SRC" "${CMAKE_SOURCE_DIR}/Yggdrasil/src/*.h" "${CMAKE_SOURCE_DIR}/Yggdrasil/src/*.cpp")
ygg_group_files("${YGG_ENGINE_SRC}")

add_library(${YGG_ENGINE} ${YGG_ENGINE_SRC})
ygg_set_dirs(${YGG_ENGINE})

# Add Precompiled Header
if (MSVC)
    set_target_properties(${YGG_ENGINE} PROPERTIES COMPILE_FLAGS "/YuYggdrasil/pch.h")
    set_source_files_properties(${CMAKE_SOURCE_DIR}/Yggdrasil/src/Yggdrasil/pch.cpp PROPERTIES COMPILE_FLAGS "/YcYggdrasil/pch.h")
endif (MSVC)

target_include_directories(${YGG_ENGINE} PUBLIC "${CMAKE_SOURCE_DIR}/Yggdrasil/src")

if(MSVC)
    target_compile_options(${YGG_ENGINE} PRIVATE "/W4" "/MP" "/std:c++17" "/WX" "/wd26812")
endif()
