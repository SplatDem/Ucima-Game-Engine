#include "vulkan_framebuffer.h"

#include "core/umemory.h"
#include "renderer/vulkan/vulkan_types.inl"

void VulkanCreateFramebuffer(
    VulkanContext *context,
    VulkanRenderpass *renderpass,
    u32 width,
    u32 height,
    u32 attachmentCount,
    VkImageView *attachments,
    VulkanFramebuffer *outFramebuffer)
{
  outFramebuffer->attachments = ualloc(sizeof(VkImageView) * attachmentCount, MEMORY_CATEGORY_RENDERER);
  for (u32 i = 0; i < attachmentCount; ++i)
    outFramebuffer->attachments[i] = attachments[i];
  outFramebuffer->renderpass = renderpass;
  outFramebuffer->attachmentCount = attachmentCount;

  VkFramebufferCreateInfo framebufferCreateInfo = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
  framebufferCreateInfo.renderPass = renderpass->handle;
  framebufferCreateInfo.attachmentCount = attachmentCount;
  framebufferCreateInfo.pAttachments = outFramebuffer->attachments;
  framebufferCreateInfo.width = width;
  framebufferCreateInfo.height = height;
  framebufferCreateInfo.layers = 1;

  VK_CHECK(vkCreateFramebuffer(context->device.device, &framebufferCreateInfo, context->allocator, &outFramebuffer->handle));
}

void VulkanDestroyFramebuffer(VulkanContext *context, VulkanFramebuffer *framebuffer) {
  vkDestroyFramebuffer(context->device.device, framebuffer->handle, context->allocator);
  if (framebuffer->attachments) {
    ufree(framebuffer->attachments, sizeof(VkImageView) * framebuffer->attachmentCount, MEMORY_CATEGORY_RENDERER);
    framebuffer->attachments = 0;
  }
  framebuffer->handle = 0;
  framebuffer->attachmentCount = 0;
  framebuffer->renderpass = 0;
}
