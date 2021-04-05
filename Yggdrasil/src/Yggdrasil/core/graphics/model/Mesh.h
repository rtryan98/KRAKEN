#pragma once
#include "Yggdrasil/Types.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace ygg::graphics
{
    namespace memory
    {
        class Buffer;
        class Texture;
    }

    struct Technique
    {
        VkPipeline pipeline;
        VkPipelineLayout layout;
    };

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;

        std::size_t hash() const;
        bool_t operator==(const Vertex& other) const;
    };

    struct Mesh
    {
        memory::Buffer* vertexBuffer;
        memory::Buffer* indexBuffer;
    };

    struct Material
    {
        memory::Texture* albedo;
    };

    struct RenderMesh
    {
        Mesh mesh{};
        Technique technique{};
        Material material{};
        glm::mat4 transform{};
    };

    struct Model
    {
        std::vector<RenderMesh> meshes{};
    };

    void createModelLoaders();
    void destroyModelLoaders();

    RenderMesh createRenderMeshFromObj(const char* filepath, Technique* technique);
    Model createModelFromObj(const char* filepath);

    void destroyRenderMesh(RenderMesh* mesh);
    void destroyModel(Model* model);
}
