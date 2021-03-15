#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/util/layers/DearImguiLayer.h"
#include "Yggdrasil/core/Globals.h"
#include "Yggdrasil/core/Application.h"
#include "Yggdrasil/core/graphics/Util.h"
#include "Yggdrasil/core/graphics/GraphicsEngine.h"

#include <vulkan/vulkan.h>
#include <imgui/backends/imgui_impl_vulkan.h>
#include <imgui/backends/imgui_impl_glfw.h>

namespace yggdrasil
{
    namespace internal
    {
        static VkDescriptorPool imguiPool;
        static VkRenderPass imguiRenderPass;
    }

    ImguiLayer::ImguiLayer(const std::string& debugName)
        : Layer(debugName)
    {}

    void ImguiLayer::init()
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

        const graphics::Context& context = globals::RENDERER->getContext();

        VkAttachmentDescription attachmentDescription{};
        attachmentDescription.format = context.screen.swapchainImageFormat;
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

        VK_CHECK(vkCreateRenderPass(context.device.logical, &renderPassInfo, graphics::VK_CPU_ALLOCATOR, &internal::imguiRenderPass));

        VkDescriptorPoolCreateInfo poolCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
        poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolCreateInfo.maxSets = maxSets;
        poolCreateInfo.poolSizeCount = static_cast<uint32_t>(std::size(poolSizes));
        poolCreateInfo.pPoolSizes = poolSizes;
        VK_CHECK(vkCreateDescriptorPool(context.device.logical, &poolCreateInfo, graphics::VK_CPU_ALLOCATOR, &internal::imguiPool));

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForVulkan(globals::APPLICATION->getWindow()->getNativeWindow(), true);
        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.Instance = context.instance;
        initInfo.Allocator = graphics::VK_CPU_ALLOCATOR;
        initInfo.PhysicalDevice = context.device.physical;
        initInfo.Device = context.device.logical;
        initInfo.Queue = context.device.queues.rasterizerQueue;
        initInfo.QueueFamily = context.device.queues.rasterizerQueueFamilyIndex;
        initInfo.ImageCount = static_cast<uint32_t>(context.screen.swapchainImages.size());
        initInfo.MinImageCount = 2;
        initInfo.DescriptorPool = internal::imguiPool;
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        ImGui_ImplVulkan_Init(&initInfo, internal::imguiRenderPass);

        VkCommandPoolCreateInfo commandPoolCreateInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        commandPoolCreateInfo.queueFamilyIndex = context.device.queues.rasterizerQueueFamilyIndex;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        VkCommandPool commandPool{};
        vkCreateCommandPool(context.device.logical, &commandPoolCreateInfo, graphics::VK_CPU_ALLOCATOR, &commandPool);

        VK_CHECK(vkResetCommandPool(context.device.logical, commandPool, 0));
        VkCommandBuffer commandBuffer{};

        VkCommandBufferAllocateInfo commandBufferAllocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandPool = commandPool;
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

        VK_CHECK(vkQueueSubmit(context.device.queues.rasterizerQueue, 1, &endInfo, VK_NULL_HANDLE));
        VK_CHECK(vkDeviceWaitIdle(context.device.logical));

        ImGui_ImplVulkan_DestroyFontUploadObjects();
        vkDestroyCommandPool(context.device.logical, commandPool, graphics::VK_CPU_ALLOCATOR);
    }

    void ImguiLayer::free()
    {
        const graphics::Context& context{ globals::RENDERER->getContext() };
        VK_CHECK(vkDeviceWaitIdle(context.device.logical));
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        vkDestroyDescriptorPool(context.device.logical, internal::imguiPool, graphics::VK_CPU_ALLOCATOR);
        vkDestroyRenderPass(context.device.logical, internal::imguiRenderPass, graphics::VK_CPU_ALLOCATOR);
    }

    void ImguiLayer::onAttach()
    {}

    void ImguiLayer::onDetach()
    {}

    void ImguiLayer::beginFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImguiLayer::endFrame()
    {
        const graphics::GraphicsEngine& renderer{ *globals::RENDERER };
        const graphics::Context& context{ renderer.getContext() };
        ImGui::Render();
        VkRenderPassBeginInfo begin{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        begin.renderPass = internal::imguiRenderPass;
        begin.framebuffer = renderer.getPerFrameData().framebuffer;
        begin.renderArea.extent = context.screen.swapchainImageExtent;
        begin.clearValueCount = 0;
        begin.pClearValues = nullptr;
        vkCmdBeginRenderPass(globals::RENDERER->getPerFrameData().commandBuffer, &begin, VK_SUBPASS_CONTENTS_INLINE);
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), globals::RENDERER->getPerFrameData().commandBuffer);
        vkCmdEndRenderPass(globals::RENDERER->getPerFrameData().commandBuffer);
    }
}
