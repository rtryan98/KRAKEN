#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/model/Mesh.h"
#include "Yggdrasil/core/graphics/Graphics.h"
#include "Yggdrasil/core/util/Log.h"

#include <filesystem>
#include <tinyobj/tiny_obj_loader.h>
#pragma warning(push, 0)
#include <glm/gtx/hash.hpp>
#pragma warning(pop)

namespace ygg::graphics
{
    void createModelLoaders()
    {
        // TODO: gltf
    }

    void destroyModelLoaders()
    {
        // TODO: gltf
    }

    std::size_t Vertex::hash() const
    {
        return (std::hash<glm::vec3>()(this->position)) ^
            (std::hash<glm::vec3>()(this->normal) << 2) ^
            (std::hash<glm::vec2>()(this->uv) << 8);
    }

    struct VertexHash
    {
        std::size_t operator()(const Vertex& k) const
        {
            return k.hash();
        };
    };

    bool_t Vertex::operator==(const Vertex& other) const
    {
        return other.position == this->position &&
            other.normal == this->normal &&
            other.uv == this->uv;
    }

    Model createModelFromObj(const char* filepath)
    {
        YGGDRASIL_CORE_TRACE("Loading Model '{0}'", filepath);

        tinyobj::attrib_t attrib{};
        std::vector<tinyobj::shape_t> shapes{};
        std::vector<tinyobj::material_t> mtl{};
        std::string warning{};
        std::string error{};

        std::filesystem::path materialPath{ filepath };
        materialPath = materialPath.parent_path();

        if (!tinyobj::LoadObj(&attrib, &shapes, &mtl, &warning, &error, filepath, materialPath.string().c_str()))
        {
            YGGDRASIL_CORE_WARN("Failed to load OBJ model '{0}'", filepath);
            return Model{};
        }

        if (!error.empty())
        {
            YGGDRASIL_CORE_ERROR(error);
        }

        Model result{};
        std::vector<std::unordered_map<Vertex, std::size_t, VertexHash>> uniqueVerticesPerGroup(mtl.size() + 1);
        std::vector<Material> materials(mtl.size());

        for (uint32_t i{ 0 }; i < mtl.size(); i++)
        {
            if (!mtl[i].diffuse_texname.empty())
            {
                materials.push_back({createTexture2DFromFile(materialPath.string().append('\\' + mtl[i].diffuse_texname).c_str())});
            }
            if (!mtl[i].normal_texname.empty())
            {
                // TODO: load normals
            }
        }

        for (auto& shape : shapes)
        {
            shape;
        }

        return result;
    }

    // TODO: deprecated
    RenderMesh createRenderMeshFromObj(const char* filepath, Technique* technique)
    {
        tinyobj::attrib_t attrib{};
        std::vector<tinyobj::shape_t> shapes{};
        std::vector<tinyobj::material_t> materials{};
        std::string warning{};
        std::string error{};

        std::filesystem::path materialPath{ filepath };
        materialPath = materialPath.parent_path();

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, filepath, materialPath.string().c_str()))
        {
            YGGDRASIL_CORE_WARN("Failed to load OBJ model '{0}'", filepath);
            return RenderMesh{};
        }

        if (!error.empty())
        {
            YGGDRASIL_CORE_ERROR(error);
        }

        YGGDRASIL_CORE_TRACE("Loaded Model `{0}`\nVertices: `{1}`\nNormals: '{2}'\nTexCoords: '{3}'\nMaterials: '{4}'\nShapes: '{5}'",
            filepath,
            attrib.vertices.size(),
            attrib.normals.size(),
            attrib.texcoords.size(),
            materials.size(),
            shapes.size());

        std::vector<uint32_t> indexData{};
        std::vector<Vertex> vertexData{};
        std::unordered_map<Vertex, uint32_t, VertexHash> uniqueVertices{};

        for (const auto& shape : shapes)
        {
            for (const auto& idx : shape.mesh.indices)
            {
                Vertex vert{};
                vert.position =
                {
                    attrib.vertices[3ull * idx.vertex_index + 0],
                    attrib.vertices[3ull * idx.vertex_index + 1],
                    attrib.vertices[3ull * idx.vertex_index + 2]
                };

                vert.normal =
                {
                    attrib.normals[3ull * idx.normal_index + 0],
                    attrib.normals[3ull * idx.normal_index + 1],
                    attrib.normals[3ull * idx.normal_index + 2]
                };

                vert.uv =
                {
                    attrib.texcoords[2ull * idx.texcoord_index + 0],
                    attrib.texcoords[2ull * idx.texcoord_index + 1]
                };

                if (uniqueVertices.count(vert) == 0)
                {
                    vertexData.push_back(vert);
                }

                indexData.push_back(uniqueVertices[vert]);
            }
        }

        RenderMesh result{};
        
        result.mesh.vertexBuffer = createBuffer(sizeof(Vertex) * static_cast<uint32_t>(vertexData.size()), memory::BUFFER_TYPE_VERTEX);
        result.mesh.indexBuffer = createBuffer(sizeof(uint32_t) * static_cast<uint32_t>(indexData.size()), memory::BUFFER_TYPE_INDEX);
        if (technique != nullptr)
        {
            result.technique = *technique;
        }

        return result;
    }

    void destroyRenderMesh(RenderMesh* mesh)
    {
        destroyBuffer(mesh->mesh.vertexBuffer);
        destroyBuffer(mesh->mesh.indexBuffer);
        destroyTexture(mesh->material.albedo);
    }

    void destroyModel(Model* model)
    {
        for (auto& mesh : model->meshes)
        {
            destroyRenderMesh(&mesh);
        }
    }
}
