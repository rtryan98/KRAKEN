#include "Yggdrasil/pch.h"

#include "Yggdrasil/core/graphics/Renderer.h"
#include "Yggdrasil/core/Globals.h"
#include "Yggdrasil/core/Application.h"
#include "Yggdrasil/core/window/Window.h"
#include "Yggdrasil/core/graphics/ShaderCompiler.h"
#include "Yggdrasil/core/graphics/PipelineFactory.h"
#include "Yggdrasil/core/graphics/memory/Resource.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <array>


namespace yggdrasil::graphics
{
    void Renderer::createPipeline()
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

    void Renderer::acquirePerFrameData()
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

    void Renderer::createDescriptorSetLayout()
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

    void Renderer::createDescriptorPool()
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

    void Renderer::freeDescriptorPool()
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

    void Renderer::prepare()
    {
        acquirePerFrameData();
        VK_CHECK(vkResetCommandPool(this->context.device.logical, this->perFrame.commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));
        VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VK_CHECK(vkBeginCommandBuffer(this->perFrame.commandBuffer, &commandBufferBeginInfo));
    }

    void Renderer::onUpdate()
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

        memory::uploadDataToUniformBuffer(&camera, sizeof(Camera), this->ubo, this->perFrame.frame, this->perFrame.commandBuffer);

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = this->ubo.ubo.buffer;
        bufferInfo.offset = this->ubo.perFrameOffset * this->perFrame.frame;
        bufferInfo.range = this->ubo.ubo.size / static_cast<uint32_t>(this->context.screen.swapchainImages.size());

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
        vkCmdBindVertexBuffers(this->perFrame.commandBuffer, 0, 1, &this->vbo.buffer, &offset);
        vkCmdDraw(this->perFrame.commandBuffer, 3, 1, 0, 0);
        vkCmdEndRenderPass(this->perFrame.commandBuffer);
    }

    void Renderer::present()
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

    void Renderer::init(const Window& window)
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

        VkMemoryPropertyFlags vboFlags
        {
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
        };
        this->vbo = memory::createAllocatedBuffer(this->context.device, vboData.size() * sizeof(float_t), vboFlags, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

        VkMemoryPropertyFlags stagingFlags
        {
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_CACHED_BIT
        };
        memory::AllocatedBuffer stagingBuffer
        { 
            memory::createAllocatedBuffer(this->context.device, vboData.size() * sizeof(float_t), stagingFlags, VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
        };

        memory::uploadDataToBuffer(stagingBuffer, vboData.data(), vboData.size() * sizeof(float_t));
        memory::copyAllocatedBuffer(this->context.device, stagingBuffer, this->vbo, this->context.commandPools[this->perFrame.frame], this->context.commandBuffers[this->perFrame.frame]);
        memory::destroyAllocatedBuffer(this->context.device, stagingBuffer);

        this->ubo = memory::createUniformBuffer(this->context, 256);

        memory::Buffer* buffer{ this->buffers.allocate() };
        buffer->create(this, memory::BUFFER_TYPE_VERTEX | memory::BUFFER_TYPE_INDEX, 0x0, 512);
        buffer->destroy(this->context.device);
    }

    void Renderer::free()
    {
        VK_CHECK(vkDeviceWaitIdle(this->context.device.logical));

        memory::destroyUniformBuffer(this->context, this->ubo);
        memory::destroyAllocatedBuffer(this->context.device, this->vbo);

        if (this->descriptorPool != VK_NULL_HANDLE)
        {
            freeDescriptorPool();
        }

        util::destroy(&this->pipeline, vkDestroyPipeline, this->context.device.logical);
        util::destroy(&this->pipelineLayout, vkDestroyPipelineLayout, this->context.device.logical);
        YGGDRASIL_CORE_TRACE("Destroying Context.");
        graphics::freeContext(this->context);
    }

    const graphics::Context& Renderer::getContext() const
    {
        return this->context;
    }

    const PerFrame& Renderer::getPerFrameData() const
    {
        return this->perFrame;
    }
}
