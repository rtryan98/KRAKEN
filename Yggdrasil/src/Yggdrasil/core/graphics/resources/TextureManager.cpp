#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/resources/TextureManager.h"
#include "Yggdrasil/core/graphics/resources/BufferManager.h"
#include "Yggdrasil/core/graphics/GraphicsEngine.h"
#include <stb/stb_image.h>

namespace ygg::graphics
{
    void TextureManager::create(GraphicsEngine* const graphicsEngine)
    {
        for (uint32_t i{ 0 }; i < graphicsEngine->getContext().screen.swapchainImages.size(); i++)
        {
            memory::Buffer* buffer
            {
                graphicsEngine->resourceManager.bufferManager.createBuffer(graphicsEngine,
                    memory::BUFFER_TYPE_STAGING, 0x0, 1024 * 1024 * 128)
            };
#if YGGDRASIL_USE_ASSERTS
            std::string stagingBufferName{ "TextureManager staging buffer " + std::to_string(i) };
            VK_SET_OBJECT_DEBUG_NAME(graphicsEngine->getContext().device,
                reinterpret_cast<uint64_t>(buffer->handle),
                VK_OBJECT_TYPE_BUFFER,
                stagingBufferName.c_str());
#endif
            this->stagingBuffers.push_back(buffer);
        }
    }

    void TextureManager::destroy(GraphicsEngine* const graphicsEngine)
    {
        for (memory::Buffer* stagingBuffer : this->stagingBuffers)
        {
            graphicsEngine->resourceManager.bufferManager.destroyBuffer(graphicsEngine, stagingBuffer);
        }
    }

    void TextureManager::handleStagedTextures(GraphicsEngine* const graphicsEngine)
    {
        if (!this->bufferToTextureCopies.empty())
        {
            std::vector<VkImageMemoryBarrier> layoutTransitionBarriers{};
            std::vector<VkImageMemoryBarrier> transferDstOptimalBarriers{};
            for (auto& copy : this->bufferToTextureCopies)
            {
                layoutTransitionBarriers.push_back(
                    {
                        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,   // sType
                        nullptr,                                  // pNext
                        VK_ACCESS_TRANSFER_WRITE_BIT,             // srcAccessMask
                        VK_ACCESS_SHADER_READ_BIT,                // dstAccessMask
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,     // oldLayout
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, // newLayout              // TODO: upload and let gpu modify textures?
                        VK_QUEUE_FAMILY_IGNORED,                  // srcQueueFamilyIndex
                        VK_QUEUE_FAMILY_IGNORED,                  // dstQueueFamilyIndex
                        copy.dst->handle,                         // image
                        {                                         // {
                            VK_IMAGE_ASPECT_COLOR_BIT,            //     subresourceRange.aspectMask;
                            0,                                    //     subresourceRange.baseMipLevel;
                            copy.dst->mipLevels,                  //     subresourceRange.levelCount;
                            0,                                    //     subresourceRange.baseArrayLayer;
                            copy.dst->layers                      //     subresourceRange.layerCount;
                        }                                         // }
                    }
                );

                transferDstOptimalBarriers.push_back(
                    {
                        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,   // sType
                        nullptr,                                  // pNext
                        0x0,                                      // srcAccessMask
                        VK_ACCESS_TRANSFER_WRITE_BIT,             // dstAccessMask
                        VK_IMAGE_LAYOUT_UNDEFINED,                // oldLayout
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,     // newLayout              // TODO: upload and let gpu modify textures?
                        VK_QUEUE_FAMILY_IGNORED,                  // srcQueueFamilyIndex
                        VK_QUEUE_FAMILY_IGNORED,                  // dstQueueFamilyIndex
                        copy.dst->handle,                         // image
                        {                                         // {
                            VK_IMAGE_ASPECT_COLOR_BIT,            //     subresourceRange.aspectMask;
                            0,                                    //     subresourceRange.baseMipLevel;
                            copy.dst->mipLevels,                  //     subresourceRange.levelCount;
                            0,                                    //     subresourceRange.baseArrayLayer;
                            copy.dst->layers                      //     subresourceRange.layerCount;
                        }                                         // }
                    }
                );

            }
            VkMemoryBarrier memoryBarrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER };
            memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            memoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

            vkCmdPipelineBarrier(graphicsEngine->getPerFrameData().commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0x0,
                0, nullptr,
                0, nullptr,
                static_cast<uint32_t>(transferDstOptimalBarriers.size()), transferDstOptimalBarriers.data());

            for (uint32_t i{ 0 }; i < this->bufferToTextureCopies.size(); i++)
            {
                memory::BufferToTextureCopy& copy{ this->bufferToTextureCopies[i] };
                copy.src->copy(copy.dst, copy.srcOffset, graphicsEngine->getPerFrameData().commandBuffer, copy.dstOffsetX, copy.dstOffsetY, copy.dstOffsetZ);
            }

            // TODO: more fine grained pipeline dst mask?
            vkCmdPipelineBarrier(graphicsEngine->getPerFrameData().commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0x0,
                1, &memoryBarrier,
                0, nullptr,
                static_cast<uint32_t>(layoutTransitionBarriers.size()), layoutTransitionBarriers.data());

            this->bufferToTextureCopies.clear();
        }
        this->currentStagingOffset = 0;
    }

    memory::Texture* TextureManager::createTexture(const GraphicsEngine* const graphicsEngine, memory::TextureType textureType,
        uint32_t width, uint32_t height, uint32_t depth, uint32_t layers, VkFormat textureFormat, memory::TextureTiling textureTiling, bool_t createTextureSampler)
    {
        memory::Texture* result{ this->texturePool.allocate() };
        result->create(graphicsEngine, textureType, width, height, depth, layers, textureFormat, textureTiling, createTextureSampler);
        return result;
    }

    void TextureManager::destroyTexture(const GraphicsEngine* const graphicsEngine, memory::Texture* texture)
    {
        texture->destroy(graphicsEngine->getContext().device);
        this->texturePool.free(texture);
    }

    void TextureManager::uploadTexture(GraphicsEngine* const graphicsEngine, memory::Texture* texture, void* textureData, uint32_t textureSize)
    {
        memory::Buffer* currentStagingBuffer{ this->stagingBuffers[graphicsEngine->getPerFrameData().frame] };
        currentStagingBuffer->upload(graphicsEngine, textureData, textureSize, this->currentStagingOffset);
        this->bufferToTextureCopies.push_back(
            {
                this->stagingBuffers[graphicsEngine->getPerFrameData().frame],
                texture,
                this->currentStagingOffset,
                0,
                0,
                0
            }
        );
        this->currentStagingOffset += textureSize;
    }

    memory::Texture* TextureManager::createTexture2DFromFile(GraphicsEngine* const graphicsEngine, const char* fileName)
    {
        memory::Texture* texture{ this->texturePool.allocate() };

        int32_t x, y, channels;
        uint8_t* textureData{ stbi_load(fileName, &x, &y, &channels, STBI_rgb_alpha) };

        VkFormat format{};

        switch (channels)
        {
        case 1:
            format = VK_FORMAT_R8_UNORM;
            break;
        case 2:
            format = VK_FORMAT_R8G8_UNORM;
            break;
        case 3:
            format = VK_FORMAT_R8G8B8_UNORM;
            break;
        default:
            format = VK_FORMAT_R8G8B8A8_UNORM;
            break;
        }

        texture = createTexture(graphicsEngine, memory::TEXTURE_TYPE_2D,
            x, y, 1, 1, format,
            memory::TextureTiling::TEXTURE_TILING_OPTIMAL);
        uploadTexture(graphicsEngine, texture, textureData, channels * sizeof(uint8_t) * x * y);
        return texture;
    }
}
