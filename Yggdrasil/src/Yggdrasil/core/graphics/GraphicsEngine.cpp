#include "Yggdrasil/pch.h"

#include "Yggdrasil/core/graphics/GraphicsEngine.h"
#include "Yggdrasil/core/Globals.h"
#include "Yggdrasil/core/Application.h"
#include "Yggdrasil/core/window/Window.h"
#include "Yggdrasil/core/graphics/ShaderCompiler.h"
#include "Yggdrasil/core/graphics/PipelineFactory.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <array>


namespace yggdrasil::graphics
{
    void GraphicsEngine::createPipeline()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pSetLayouts = &this->descriptorSetLayout;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
        VK_CHECK(vkCreatePipelineLayout(this->context.device.logical, &pipelineLayoutCreateInfo, graphics::VK_CPU_ALLOCATOR, &this->pipelineLayout));

        graphics::GraphicsPipelineFactory factory{};
        factory.defaults(this->context);

        VkVertexInputAttributeDescription attributeDescription{};
        attributeDescription.binding = 0;
        attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescription.location = 0;
        attributeDescription.offset = 0;

        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        bindingDescription.stride = 3 * sizeof(float_t);

        factory.vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 1;
        factory.vertexInputStateCreateInfo.pVertexAttributeDescriptions = &attributeDescription;
        factory.vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        factory.vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;

        std::vector<uint32_t> fragment{ shadercompiler::compileGlsl("res/shader/main.frag") };
        factory.pushShaderStage(this->context, fragment, VK_SHADER_STAGE_FRAGMENT_BIT);
        std::vector<uint32_t> vertex{ shadercompiler::compileGlsl("res/shader/main.vert") };
        factory.pushShaderStage(this->context, vertex, VK_SHADER_STAGE_VERTEX_BIT);
        this->pipeline = factory.createPipeline(this->context, this->context.screen.swapchainRenderPass, this->pipelineLayout);
        factory.clear(this->context);
    }

    void GraphicsEngine::acquirePerFrameData()
    {
        this->perFrame.acquireSemaphore = this->context.syncObjects.acquireSemaphores[this->perFrame.frame];
        this->perFrame.releaseSemaphore = this->context.syncObjects.releaseSemaphores[this->perFrame.frame];
        this->perFrame.acquireFence = this->context.syncObjects.acquireFences[this->perFrame.frame];

        vkWaitForFences(this->context.device.logical, 1, &this->perFrame.acquireFence, VK_TRUE, ~0ull);

        uint32_t imageIndex{};
        VkResult acquireNexImageResult{ vkAcquireNextImageKHR(this->context.device.logical, this->context.screen.swapchain, ~0ull, this->perFrame.acquireSemaphore, VK_NULL_HANDLE, &imageIndex) };
        if (acquireNexImageResult == VK_ERROR_OUT_OF_DATE_KHR || acquireNexImageResult == VK_SUBOPTIMAL_KHR)
        {
            YGGDRASIL_CORE_TRACE("Recreating swapchain - vkAcquireNextImageKHR");
            this->context.screen.recreateSwapchain(this->context.device);
        }
        else if(acquireNexImageResult != VK_SUCCESS)
        {
            VK_CHECK(acquireNexImageResult);
        }

        this->perFrame.frame = imageIndex;
        this->perFrame.commandPool = this->context.commandPools[imageIndex];
        this->perFrame.commandBuffer = this->context.commandBuffers[imageIndex];
        this->perFrame.swapchainImage = this->context.screen.swapchainImages[imageIndex];
        this->perFrame.swapchainImageView = this->context.screen.swapchainImageViews[imageIndex];
        this->perFrame.framebuffer = this->context.screen.swapchainFramebuffers[imageIndex];
    }

    void GraphicsEngine::createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding binding{};
        binding.binding = 0;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        binding.descriptorCount = 1;
        binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        binding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo createInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        createInfo.bindingCount = 1;
        createInfo.pBindings = &binding;

        VK_CHECK(vkCreateDescriptorSetLayout(this->context.device.logical, &createInfo, VK_CPU_ALLOCATOR, &this->descriptorSetLayout));
    }

    void GraphicsEngine::createDescriptorPool()
    {
        VkDescriptorPoolSize size{};
        size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        size.descriptorCount = static_cast<uint32_t>(this->context.screen.swapchainImages.size());

        VkDescriptorPoolCreateInfo createInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        createInfo.poolSizeCount = 1;
        createInfo.pPoolSizes = &size;
        createInfo.maxSets = static_cast<uint32_t>(this->context.screen.swapchainImages.size());

        for (uint32_t i{ 0 }; i < this->context.screen.swapchainImages.size(); i++)
        {
            this->descriptorSets.push_back(VkDescriptorSet{});
        }

        VK_CHECK(vkCreateDescriptorPool(this->context.device.logical, &createInfo, VK_CPU_ALLOCATOR, &this->descriptorPool));

        std::vector<VkDescriptorSetLayout> setLayouts(this->context.screen.swapchainImages.size(), this->descriptorSetLayout);

        VkDescriptorSetAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        allocateInfo.descriptorSetCount = static_cast<uint32_t>(this->context.screen.swapchainImages.size());
        allocateInfo.pSetLayouts = setLayouts.data();
        allocateInfo.descriptorPool = this->descriptorPool;

        VK_CHECK(vkAllocateDescriptorSets(this->context.device.logical, &allocateInfo, this->descriptorSets.data()));
    }

    void GraphicsEngine::freeDescriptorPool()
    {
        if (this->descriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(this->context.device.logical, this->descriptorPool, VK_CPU_ALLOCATOR);
        }
        if (this->descriptorSetLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(this->context.device.logical, this->descriptorSetLayout, VK_NULL_HANDLE);
        }
    }

    void GraphicsEngine::prepare()
    {
        acquirePerFrameData();
        VK_CHECK(vkResetCommandPool(this->context.device.logical, this->perFrame.commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));
        VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VK_CHECK(vkBeginCommandBuffer(this->perFrame.commandBuffer, &commandBufferBeginInfo));
        handleStagedData();
    }

    void GraphicsEngine::handleStagedData()
    {
        handleStagedBufferCopies();
        handleStagedBufferToTextureCopies();
    }

    void GraphicsEngine::handleStagedBufferCopies()
    {
        if (!this->bufferCopyQueue.empty())
        {
            while (!this->bufferCopyQueue.empty())
            {
                memory::BufferCopy& copy{ this->bufferCopyQueue.front() };
                copy.src->copy(copy.dst, copy.srcOffset, copy.dstOffset, copy.src->size, this->perFrame.commandBuffer);
                this->bufferCopyQueue.pop();
            }
            VkMemoryBarrier stagingBarrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER };
            stagingBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            stagingBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            // TODO: Use more fine grained synchronization. That means VBO data -> VK_PIPELINE_STAGE_VERTEX_INPUT_STAGE etc.
            vkCmdPipelineBarrier(this->perFrame.commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_DEPENDENCY_BY_REGION_BIT,
                1, &stagingBarrier,
                0, nullptr,
                0, nullptr);
        }
    }

    void GraphicsEngine::handleStagedBufferToTextureCopies()
    {
        if (!this->bufferToTextureCopyQueue.empty())
        {
            std::vector<VkImageMemoryBarrier> layoutTransitionBarriers{};
            layoutTransitionBarriers.reserve(32);
            while (!this->bufferCopyQueue.empty())
            {
                memory::BufferToTextureCopy& copy{ this->bufferToTextureCopyQueue.front() };
                copy.src->copy(copy.dst, copy.srcOffset, this->perFrame.commandBuffer, copy.dstOffsetX, copy.dstOffsetY, copy.dstOffsetZ );

                VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.image = copy.dst->handle;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                barrier.subresourceRange.layerCount = copy.dst->layers;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.levelCount = copy.dst->mipLevels;
                barrier.subresourceRange.baseMipLevel = 0;

                layoutTransitionBarriers.push_back( 
                    {
                        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,   // sType
                        nullptr,                                  // pNext
                        VK_ACCESS_TRANSFER_WRITE_BIT,             // srcAccessMask
                        VK_ACCESS_SHADER_READ_BIT,                // dstAccessMask
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,     // oldLayout
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, // newLayout
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
                this->bufferToTextureCopyQueue.pop();
            }
            VkMemoryBarrier memoryBarrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER };
            memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(this->perFrame.commandBuffer, VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_SHADER_READ_BIT, VK_DEPENDENCY_BY_REGION_BIT,
                1, &memoryBarrier,
                0, nullptr,
                static_cast<uint32_t>(layoutTransitionBarriers.size()), layoutTransitionBarriers.data());
        }
    }

    void GraphicsEngine::onUpdate()
    {
        glm::vec3 forward{ 0.0f, 0.0f, -1.0f };
        glm::vec3 up{ 0.0f, 1.0f, 0.0f };
        glm::vec3 pos{ 0.0f, 0.0f, 1.0f + glm::sin(glfwGetTime()) };

        struct Camera
        {
            glm::mat4 projection{ glm::perspectiveFov<float_t>(glm::radians(75.0f), 1920.0f, 1080.0f, 0.001f, 1000.0f) };
            glm::mat4 view{ 1.0f };
        } camera;
        camera.view = glm::lookAt(pos, forward, up);
        this->uniformBuffer->upload(this, &camera, sizeof(camera), 0);

        VkDescriptorBufferInfo bufferInfo{};

        bufferInfo.buffer = this->uniformBuffer->handle;
        bufferInfo.offset = this->uniformBuffer->size * this->perFrame.frame;
        bufferInfo.range  = this->uniformBuffer->size;

        VkWriteDescriptorSet descriptorWrite{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstSet = this->descriptorSets[this->perFrame.frame];
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.dstArrayElement = 0;

        vkUpdateDescriptorSets(this->context.device.logical, 1, &descriptorWrite, 0, nullptr);

        VkClearValue clearValue{};
        clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };

        VkRenderPassBeginInfo renderPassBeginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        renderPassBeginInfo.framebuffer = this->perFrame.framebuffer;
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearValue;
        renderPassBeginInfo.renderPass = this->context.screen.swapchainRenderPass;
        renderPassBeginInfo.renderArea.extent = this->context.screen.swapchainImageExtent;
        renderPassBeginInfo.renderArea.offset = {0, 0};
        vkCmdBeginRenderPass(this->perFrame.commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(this->perFrame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline);
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = static_cast<float>(context.screen.swapchainImageExtent.height);
        viewport.width = static_cast<float>(context.screen.swapchainImageExtent.width);
        viewport.height = -static_cast<float>(context.screen.swapchainImageExtent.height);
        viewport.maxDepth = 1.0f;
        viewport.minDepth = 0.0f;
        vkCmdSetViewport(this->perFrame.commandBuffer, 0, 1, &viewport);
        VkDeviceSize offset{};
        vkCmdBindDescriptorSets(this->perFrame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayout,
            0, 1, &this->descriptorSets[this->perFrame.frame], 0, nullptr);
        vkCmdBindVertexBuffers(this->perFrame.commandBuffer, 0, 1, &this->vertexBuffer->handle, &offset);
        vkCmdDraw(this->perFrame.commandBuffer, 3, 1, 0, 0);
        vkCmdEndRenderPass(this->perFrame.commandBuffer);
    }

    void GraphicsEngine::present()
    {
        VkImageMemoryBarrier layoutBarrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        layoutBarrier.image = this->perFrame.swapchainImage;
        layoutBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        layoutBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        layoutBarrier.srcQueueFamilyIndex = this->context.device.queues.rasterizerQueueFamilyIndex;
        layoutBarrier.dstQueueFamilyIndex = this->context.device.queues.rasterizerQueueFamilyIndex;
        layoutBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        layoutBarrier.subresourceRange.layerCount = 1;
        layoutBarrier.subresourceRange.baseArrayLayer = 0;
        layoutBarrier.subresourceRange.levelCount = 1;
        layoutBarrier.subresourceRange.baseMipLevel = 0;

        vkCmdPipelineBarrier(this->perFrame.commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0,
            0, nullptr, 0, nullptr, 1, &layoutBarrier);

        VK_CHECK(vkEndCommandBuffer(this->perFrame.commandBuffer));
        VkPipelineStageFlags submitStageMask{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &this->perFrame.acquireSemaphore;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &this->perFrame.releaseSemaphore;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &this->perFrame.commandBuffer;
        submitInfo.pWaitDstStageMask = &submitStageMask;

        VK_CHECK(vkResetFences(this->context.device.logical, 1, &this->perFrame.acquireFence));
        VK_CHECK(vkQueueSubmit(this->context.device.queues.rasterizerQueue, 1, &submitInfo, this->perFrame.acquireFence));

        VkPresentInfoKHR queuePresentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        queuePresentInfo.swapchainCount = 1;
        queuePresentInfo.pSwapchains = &this->context.screen.swapchain;
        queuePresentInfo.waitSemaphoreCount = 1;
        queuePresentInfo.pWaitSemaphores = &this->perFrame.releaseSemaphore;
        queuePresentInfo.pImageIndices = &this->perFrame.frame;

        VkResult presentResult{ vkQueuePresentKHR(this->context.device.queues.presentQueue, &queuePresentInfo) };
        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR)
        {
            YGGDRASIL_CORE_TRACE("Recreating swapchain - vkQueuePresentKHR");
            this->context.screen.recreateSwapchain(this->context.device);
        }
        else if(presentResult != VK_SUCCESS)
        {
            VK_CHECK(presentResult);
        }
        this->perFrame.frame = (this->perFrame.frame + 1) % static_cast<uint32_t>(this->context.screen.swapchainImages.size());
    }

    void GraphicsEngine::init(const Window& window)
    {
        YGGDRASIL_UNUSED_VARIABLE(window);
        graphics::initContext(this->context);
        createDescriptorSetLayout();
        createPipeline();
        createDescriptorPool();

        std::array<float_t, 9> vboData
        {
            -0.5f, -0.5f, -1.5f,
             0.5f, -0.5f, -1.5f,
            -0.5f,  0.5f, -1.5f
        };

        this->uniformBuffer = this->buffers.allocate();
        this->uniformBuffer->create(this, memory::BUFFER_TYPE_UNIFORM, memory::BUFFER_USAGE_UPDATE_EVERY_FRAME, 128);

        this->vertexBuffer = this->buffers.allocate();
        this->vertexBuffer->create(this, memory::BUFFER_TYPE_VERTEX, 0x0, 256);

        this->stagingBuffer = this->buffers.allocate();
        this->stagingBuffer->create(this, memory::BUFFER_TYPE_STAGING, 0, 256);
        this->stagingBuffer->upload(this, &vboData, vboData.size() * sizeof(float_t), 0);

        stageBufferCopy(this->stagingBuffer, this->vertexBuffer);

        memory::Texture* texture{ this->images.allocate() };
        texture->create(this, memory::TEXTURE_TYPE_2D,
            100, 100, 1, 1, VK_FORMAT_R8G8B8A8_UNORM);
        texture->destroy(this->context.device);
    }

    void GraphicsEngine::free()
    {
        VK_CHECK(vkDeviceWaitIdle(this->context.device.logical));

        this->uniformBuffer->destroy(this->context.device);
        this->vertexBuffer->destroy(this->context.device);
        this->stagingBuffer->destroy(this->context.device);

        if (this->descriptorPool != VK_NULL_HANDLE)
        {
            freeDescriptorPool();
        }

        util::destroy(&this->pipeline, vkDestroyPipeline, this->context.device.logical);
        util::destroy(&this->pipelineLayout, vkDestroyPipelineLayout, this->context.device.logical);
        YGGDRASIL_CORE_TRACE("Destroying Context.");
        graphics::freeContext(this->context);
    }

    const graphics::Context& GraphicsEngine::getContext() const
    {
        return this->context;
    }

    const PerFrame& GraphicsEngine::getPerFrameData() const
    {
        return this->perFrame;
    }

    void GraphicsEngine::stageBufferCopy(memory::Buffer* src, memory::Buffer* dst, uint64_t srcOffset, uint64_t dstOffset)
    {
        this->bufferCopyQueue.push({src, dst, srcOffset, dstOffset});
    }

    void GraphicsEngine::stageBufferToImageCopy(memory::Buffer* src, memory::Texture* dst,
        uint32_t srcOffset, uint32_t dstOffsetX, uint32_t dstOffsetY, uint32_t dstOffsetZ)
    {
        this->bufferToTextureCopyQueue.push({src, dst, srcOffset, dstOffsetX, dstOffsetY, dstOffsetZ});
    }
}
