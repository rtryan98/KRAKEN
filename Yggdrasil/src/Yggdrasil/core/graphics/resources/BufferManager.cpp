#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/graphics/resources/BufferManager.h"
#include "Yggdrasil/core/graphics/GraphicsEngine.h"

#include <vulkan/vulkan.h>

namespace ygg::graphics
{
    void BufferManager::create(const GraphicsEngine* const graphicsEngine)
    {
        for (uint32_t i{ 0 }; i < graphicsEngine->getContext().screen.swapchainImages.size(); i++)
        {
            memory::Buffer* buffer{ this->bufferPool.allocate() };
            buffer->create(graphicsEngine, memory::BUFFER_TYPE_STAGING, 0x0, 1024 * 1024 * 128);
#if YGGDRASIL_USE_ASSERTS
            std::string stagingBufferName{ "BufferManager staging buffer " + std::to_string(i) };
            VK_SET_OBJECT_DEBUG_NAME(graphicsEngine->getContext().device,
                reinterpret_cast<uint64_t>(buffer->handle),
                VK_OBJECT_TYPE_BUFFER,
                stagingBufferName.c_str());
#endif
            this->stagingBuffers.push_back( buffer );
        }
    }

    void BufferManager::destroy(const GraphicsEngine* const graphicsEngine)
    {
        for (memory::Buffer* stagingBuffer : this->stagingBuffers)
        {
            stagingBuffer->destroy(graphicsEngine->getContext().device);
        }
    }

    void BufferManager::handleStagedBuffers(const GraphicsEngine* const graphicsEngine)
    {
        if (!this->stagedBufferCopies.empty())
        {
            for (auto& copy : this->stagedBufferCopies)
            {
                copy.src->copy(copy.dst, copy.srcOffset, copy.dstOffset, copy.size, graphicsEngine->getPerFrameData().commandBuffer);
            }
            this->stagedBufferCopies.clear();

            VkMemoryBarrier stagingBarrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER };
            stagingBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            stagingBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            // TODO: Use more fine grained synchronization. That means VBO data -> VK_PIPELINE_STAGE_VERTEX_INPUT_STAGE etc.
            vkCmdPipelineBarrier(graphicsEngine->getPerFrameData().commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0x0,
                1, &stagingBarrier,
                0, nullptr,
                0, nullptr);
        }
        this->currentStagingOffset = 0;
    }

    memory::Buffer* BufferManager::createBuffer(const GraphicsEngine* const graphicsEngine, uint32_t bufferType, uint32_t bufferUsage, uint64_t bufferSize)
    {
        memory::Buffer* result{ this->bufferPool.allocate() };
        result->create(graphicsEngine, bufferType, bufferUsage, bufferSize);
        return result;
    }

    void BufferManager::uploadDataToBuffer(const GraphicsEngine* const graphicsEngine, memory::Buffer* target, void* bufferData, uint64_t dataSize, uint64_t bufferOffset)
    {
        if ((target->usage & memory::BUFFER_USAGE_UPDATE_EVERY_FRAME) ||
            (target->type & memory::BUFFER_TYPE_UNIFORM))
        {
            target->upload(graphicsEngine, bufferData, dataSize, bufferOffset);
        }
        else
        {
            memory::Buffer* currentStagingBuffer{ this->stagingBuffers[graphicsEngine->getPerFrameData().frame] };
            currentStagingBuffer->upload(graphicsEngine, bufferData, dataSize, this->currentStagingOffset);
            this->stagedBufferCopies.push_back(
                {
                    currentStagingBuffer,
                    target,
                    this->currentStagingOffset,
                    bufferOffset,
                    dataSize
                }
            );
            this->currentStagingOffset += dataSize;
        }
    }

    void BufferManager::destroyBuffer(const GraphicsEngine* const graphicsEngine, memory::Buffer* buffer)
    {
        buffer->destroy(graphicsEngine->getContext().device);
        this->bufferPool.free(buffer);
    }
}
