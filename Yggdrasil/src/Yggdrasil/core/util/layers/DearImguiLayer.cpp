#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/util/layers/DearImguiLayer.h"
#include "Yggdrasil/core/Globals.h"
#include "Yggdrasil/core/Application.h"
#include "Yggdrasil/core/graphics/vulkan/Util.h"
#include "Yggdrasil/core/graphics/Renderer.h"

#include <vulkan/vulkan.h>
#include <imgui/backends/imgui_impl_vulkan.h>
#include <imgui/backends/imgui_impl_glfw.h>

namespace yggdrasil
{
    ImguiLayer::ImguiLayer(const std::string& debugName)
        : Layer(debugName)
    {}

    void ImguiLayer::onAttach()
    {
        constexpr uint32_t maxSets{ 1024 };

        VkDescriptorPoolSize poolSizes[]
        {
        { VK_DESCRIPTOR_TYPE_SAMPLER, maxSets },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxSets },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, maxSets },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, maxSets },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, maxSets },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, maxSets },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxSets },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, maxSets },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, maxSets },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, maxSets },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, maxSets }
        };

        const vulkan::Context& context = globals::RENDERER->getContext();

        VkDescriptorPoolCreateInfo poolCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolCreateInfo.maxSets = maxSets;
        poolCreateInfo.poolSizeCount = static_cast<uint32_t>(std::size(poolSizes));
        poolCreateInfo.pPoolSizes = poolSizes;
        VK_CHECK(vkCreateDescriptorPool(context.device.logical, &poolCreateInfo, vulkan::VK_CPU_ALLOCATOR, &this->imguiPool));

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForVulkan(globals::APPLICATION->getWindow()->getNativeWindow(), true);
        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.Allocator = vulkan::VK_CPU_ALLOCATOR;
        initInfo.PhysicalDevice = context.device.physical;
        initInfo.Device = context.device.logical;
        initInfo.Queue = context.queues.rasterizerQueue;
        initInfo.QueueFamily = context.queues.rasterizerQueueFamilyIndex;
        initInfo.ImageCount = static_cast<uint32_t>(context.swapchainImages.size());
        initInfo.MinImageCount = 2;
        initInfo.DescriptorPool = this->imguiPool;
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        ImGui_ImplVulkan_Init(&initInfo, globals::RENDERER->getRenderPass());

        VK_CHECK(vkResetCommandPool(context.device.logical, context.commandPool, 0));
        VkCommandBuffer commandBuffer{};

        VkCommandBufferAllocateInfo commandBufferAllocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandPool = context.commandPool;
        commandBufferAllocateInfo.commandBufferCount = 1;

        VK_CHECK(vkAllocateCommandBuffers(context.device.logical, &commandBufferAllocateInfo, &commandBuffer));

        VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

        VK_CHECK(vkEndCommandBuffer(commandBuffer));

        VkSubmitInfo endInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        endInfo.commandBufferCount = 1;
        endInfo.pCommandBuffers = &commandBuffer;

        VK_CHECK(vkQueueSubmit(context.queues.rasterizerQueue, 1, &endInfo, VK_NULL_HANDLE));
        VK_CHECK(vkDeviceWaitIdle(context.device.logical));

        ImGui_ImplVulkan_DestroyFontUploadObjects();

        VkAttachmentDescription attachmentDescription{};
        attachmentDescription.format = context.swapchainImageFormat;
        attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachment{};
        colorAttachment.attachment = 0;
        colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachment;

        VkRenderPassCreateInfo renderPassInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &attachmentDescription;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        VK_CHECK(vkCreateRenderPass(context.device.logical, &renderPassInfo, vulkan::VK_CPU_ALLOCATOR, &this->imguiRenderPass));

        postAttach();
    }

    void ImguiLayer::onDetach()
    {
        const vulkan::Context& context{ globals::RENDERER->getContext() };
        preDetach();
        VK_CHECK(vkDeviceWaitIdle(context.device.logical));
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        vkDestroyDescriptorPool(context.device.logical, this->imguiPool, vulkan::VK_CPU_ALLOCATOR);
        vkDestroyRenderPass(context.device.logical, this->imguiRenderPass, vulkan::VK_CPU_ALLOCATOR);
    }

    void ImguiLayer::beginFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImguiLayer::endFrame()
    {
        const Renderer& renderer{ *globals::RENDERER };
        const vulkan::Context& context{ renderer.getContext() };
        ImGui::Render();
        VkRenderPassBeginInfo begin{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        begin.renderPass = this->imguiRenderPass;
        begin.framebuffer = renderer.getFramebuffers().at(renderer.getCurrentFrame());
        begin.renderArea.extent = context.swapchainImageExtent;
        begin.clearValueCount = 0;
        begin.pClearValues = nullptr;
        vkCmdBeginRenderPass(globals::RENDERER->getPerFrameData().commandBuffer, &begin, VK_SUBPASS_CONTENTS_INLINE);
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), globals::RENDERER->getPerFrameData().commandBuffer);
        vkCmdEndRenderPass(globals::RENDERER->getPerFrameData().commandBuffer);
    }
}
