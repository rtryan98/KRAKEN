<img align="left" src="docs/res/icon/ygg256.png" width="128px"/>

# Yggdrasil

[![Licence: BSD-3-Clause](https://img.shields.io/badge/License-BSD--3--Clause-orange.svg)](https://opensource.org/licenses/BSD-3-Clause)
![Platform: Win-64](https://img.shields.io/badge/Platform-Win--64-blue)

<br/>

*C++17 / Vulkan 1.2.170 Windows-Only Game Engine with a heavy emphasis on open world terrain rendering.*

## Disclaimer
This is a personal project to learn Vulkan, Graphics and modern technologies.
Due to this, there will be no contributions from other persons.
It makes use of many of Vulkan 1.2's core features so it is expected to have them available.

The currently used Vulkan extensions are:
* [`VK_NV_mesh_shader`](https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VK_NV_mesh_shader.html)

Extensions are mandatory unless they are tagged to have a fallback implementation.
The engine will terminate if an extension is not present and no fallback path exists.
Currently there are no fallback paths planned, nor is it planned to extend this renderer / engine to other operating systems.
For every vendor extension used, if a cross vendor solution appears it will be replaced with that one.

## Building
### Build Requirements
* CMake 3.18
* Vulkan SDK 1.2.170.0 or higher
* Visual Studio 2019

### Build Instructions
* Go to scripts/
* Run `GenerateVS2019.bat`
* Open the `.sln`-file which was generated in `build/`.
* Build the Solution.
