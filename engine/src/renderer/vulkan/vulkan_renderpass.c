#include "defines.h"
#include "core/umemory.h"
#include "vulkan_types.inl"
#include "vulkan_renderpass.h"
#include <stdio.h>

void VulkanCreateRenderpass(
    VulkanContext *context,
    VulkanRenderpass *outRenderpass,
    f32 x, f32 y, f32 w, f32 h,
    f32 r, f32 g, f32 b, f32 a,
    f32 depth,
    u32 stencil)
{
  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  u32 attachmentDescriptionCount = 2;
  VkAttachmentDescription attachmentDescriptions[attachmentDescriptionCount];

  #define CA colorAttachment

  VkAttachmentDescription CA;
  CA.format = context->swapchain.imageFormat.format;
  CA.samples = VK_SAMPLE_COUNT_1_BIT;
  CA.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  CA.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  CA.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  CA.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  CA.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  CA.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  CA.flags = 0;

  attachmentDescriptions[0] = CA;

  #define CAR colorAttachmentReference
  
  VkAttachmentReference CAR;
  CAR.attachment = 0;
  CAR.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &CAR;

  #define DA depthAttachment

  VkAttachmentDescription DA = {};
  DA.format = context->device.depthFormat;
  DA.samples = VK_SAMPLE_COUNT_1_BIT;
  DA.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  DA.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  DA.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  DA.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  DA.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  DA.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  attachmentDescriptions[1] = DA;

  #define depthAttachmentReference DAR

  VkAttachmentReference DAR;
  DAR.attachment = 1;
  DAR.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  // TODO: other attachment types
  subpass.pDepthStencilAttachment = &DAR;
  subpass.inputAttachmentCount = 0;
  subpass.pInputAttachments = 0;
  subpass.pResolveAttachments = 0;
  subpass.pPreserveAttachments = 0;

  VkSubpassDependency dep;
  dep.srcSubpass = VK_SUBPASS_EXTERNAL;
  dep.dstSubpass = 0;
  dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.srcAccessMask = 0;
  dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dep.dependencyFlags = 0;

  VkRenderPassCreateInfo renderPassCreateInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
  renderPassCreateInfo.attachmentCount = attachmentDescriptionCount;
  renderPassCreateInfo.pAttachments = attachmentDescriptions;
  renderPassCreateInfo.subpassCount = 1;
  renderPassCreateInfo.pSubpasses = &subpass;
  renderPassCreateInfo.dependencyCount = 1;
  renderPassCreateInfo.pDependencies = &dep;
  renderPassCreateInfo.pNext = 0;
  renderPassCreateInfo.flags = 0;

  VK_CHECK(vkCreateRenderPass(
        context->device.device,
        &renderPassCreateInfo,
        context->allocator,
        &outRenderpass->handle));
}

void VulkanDestroyRenderpass(
    VulkanContext *context,
    VulkanRenderpass *renderpass) 
{
  if (renderpass && renderpass->handle) {
    vkDestroyRenderPass(context->device.device, renderpass->handle, context->allocator);
    renderpass->handle = 0;
  }
}

void VulkanBeginRenderpass(
    VulkanCommandBuffer *commandBuffer,
    VulkanRenderpass *renderpass,
    VkFramebuffer frameBuffer) 
{
  VkRenderPassBeginInfo beginInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
  beginInfo.renderPass = renderpass->handle;
  beginInfo.framebuffer = frameBuffer;
  beginInfo.renderArea.offset.x = renderpass->x;
  beginInfo.renderArea.offset.y = renderpass->y;
  beginInfo.renderArea.extent.width = renderpass->w;
  beginInfo.renderArea.extent.height = renderpass->h;

  // Clear value count 2
  VkClearValue clearValues[2];
  uzeromem(clearValues, sizeof(VkClearValue) * 2);
  clearValues[0].color.float32[0] = renderpass->r;
  clearValues[0].color.float32[1] = renderpass->g;
  clearValues[0].color.float32[2] = renderpass->b;
  clearValues[0].color.float32[3] = renderpass->a;
  clearValues[1].depthStencil.depth = renderpass->depth;
  clearValues[1].depthStencil.stencil = renderpass->stencil;

  beginInfo.clearValueCount = 2;
  beginInfo.pClearValues = clearValues;

  vkCmdBeginRenderPass(commandBuffer->handle, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
  commandBuffer->state = COMMAND_BUFFER_STATE_IN_RENDER_PASS;
}

void VulkanEndRenderpass(
  VulkanCommandBuffer *commandBuffer,
  VulkanRenderpass *renderpass)
{
  vkCmdEndRenderPass(commandBuffer->handle);
  commandBuffer->state = COMMAND_BUFFER_STATE_RECORDING;
}
