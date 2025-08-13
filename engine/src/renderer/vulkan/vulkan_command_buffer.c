#include "vulkan_command_buffer.h"
#include "core/umemory.h"

void VulkanAllocateCommandBuffer(
    VulkanContext *context,
    VkCommandPool pool,
    BOOLEAN isPrimary,
    VulkanCommandBuffer *outCommandBuffer) 
{
  uzeromem(outCommandBuffer, sizeof(&outCommandBuffer));

  VkCommandBufferAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
  allocInfo.commandPool = pool;
  allocInfo.level = isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
  allocInfo.commandBufferCount = 1;
  allocInfo.pNext = 0;

  outCommandBuffer->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
  VK_CHECK(vkAllocateCommandBuffers(context->device.device, &allocInfo, &outCommandBuffer->handle));

  outCommandBuffer->state = COMMAND_BUFFER_STATE_READY;
}

void VulkanFreeCommandBuffer(
    VulkanContext *context,
    VkCommandPool pool,
    VulkanCommandBuffer *commandBuffer) 
{
  vkFreeCommandBuffers(context->device.device, pool, 1, &commandBuffer->handle);
  commandBuffer->handle = 0;
  commandBuffer->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
}

void VulkanBeginCommandBuffer(
    VulkanCommandBuffer *commandBuffer,
    BOOLEAN isSingleuse,
    BOOLEAN isRenderpassContinue,
    BOOLEAN isSimultaneousUse) 
{
  VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
  beginInfo.flags = 0;
  if (isSingleuse)
    beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  if (isRenderpassContinue)
    beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
  if (isSimultaneousUse)
    beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

  VK_CHECK(vkBeginCommandBuffer(commandBuffer->handle, &beginInfo));

  commandBuffer->state = COMMAND_BUFFER_STATE_RECORDING;
}

void VulkanEndCommandBuffer(VulkanCommandBuffer *commandBuffer)
{
  VK_CHECK(vkEndCommandBuffer(commandBuffer->handle));
  commandBuffer->state = COMMAND_BUFFER_STATE_RECORDING_ENDED;
}

void VulkanUpdateCommandBufferSubmitted(VulkanCommandBuffer *commandBuffer)
{
  commandBuffer->state = COMMAND_BUFFER_STATE_SUBMITTED;
}

void VulkanResetCommandBuffer(VulkanCommandBuffer *commandBuffer)
{
  commandBuffer->state = COMMAND_BUFFER_STATE_READY;
}

void VulkanCommandBufferSingleUse(
    VulkanContext *context,
    VkCommandPool pool,
    VulkanCommandBuffer *commandBuffer) 
{
  VulkanAllocateCommandBuffer(context, pool, TRUE, commandBuffer);
  VulkanBeginCommandBuffer(commandBuffer, TRUE, FALSE, FALSE);
}

void VulkanCommandBufferEndSingleUse(
    VulkanContext *context,
    VkCommandPool pool,
    VulkanCommandBuffer *commandBuffer,
    VkQueue queue) 
{
  VulkanEndCommandBuffer(commandBuffer);
  VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer->handle;
  VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, 0));
  VK_CHECK(vkQueueWaitIdle(queue));
  VulkanFreeCommandBuffer(context, pool, commandBuffer);
}
