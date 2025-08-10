#include "vulkan_device.h"
#include "core/umemory.h"
#include "core/ustring.h"
#include "containers/darray.h"
#include "defines.h"
#include "renderer/vulkan/vulkan_types.inl"
#include <stdio.h>
#include <vulkan/vulkan_core.h>

typedef struct VulkanPhysicalDeviceRequirements {
  b8 graphics;
  b8 present;
  b8 compute;
  b8 transfer;
  const char **deviceExtensionNames;
  b8 samplerAnisotropy;
  b8 discreteGPU;
} VulkanPhysicalDeviceRequirements;

typedef struct VulkanPhysicalDeviceQueueFamilyInfo {
  u32 graphicsFamilyIndex;
  u32 presentFamilyIndex;
  u32 computeFamilyIndex;
  u32 transferFamilyIndex;
} VulkanPhysicalDeviceQueueFamilyInfo;

BOOLEAN SelectPhysicalDevice(VulkanContext *context);

// This is not my function name
BOOLEAN PhysicalDeviceMeetsRequirements(
  VkPhysicalDevice device,
  VkSurfaceKHR surface,
  const VkPhysicalDeviceProperties *properties,
  const VkPhysicalDeviceFeatures *features,
  const VulkanPhysicalDeviceRequirements *requirements,
  VulkanPhysicalDeviceQueueFamilyInfo *outQueueFamilyInfo,
  VulkanSwapchainSupportInfo *outSwapchainSupport);

BOOLEAN VulkanCreateDevice(VulkanContext *context) {
  if (!SelectPhysicalDevice(context)) return FALSE;

  S_TraceLogInfo("Creating logical device...");
  b8 presentSharesGraphicsQueue = context->device.graphicsQueueIndex == context->device.presentQueueIndex;
  b8 transferSharesGraphicsQueue = context->device.graphicsQueueIndex == context->device.transferQueueIndex;
  u32 indexCount = 1;
  if (!transferSharesGraphicsQueue)
    indexCount++;

  u32 indices[indexCount];
  u8 index = 0;
  indices[index++] = context->device.graphicsQueueIndex;
  if (!presentSharesGraphicsQueue)
    indices[index++] = context->device.presentQueueIndex;
  if (!transferSharesGraphicsQueue)
    indices[index++] = context->device.transferQueueIndex;

  // Queue Long To Write
  #define QLTW queueCreateInfos[i]

  VkDeviceQueueCreateInfo queueCreateInfos[indexCount];
  for (u32 i = 0; i < indexCount; ++i) {
    QLTW.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    QLTW.queueFamilyIndex = indices[i];
    QLTW.queueCount = 1;
    if (indices[i] == context->device.graphicsQueueIndex)
      QLTW.queueCount = 2;
    QLTW.flags = 0;
    QLTW.pNext = 0;
    f32 queuePriory = 1.0f;
    QLTW.pQueuePriorities = &queuePriory;
  }

  VkPhysicalDeviceFeatures deviceFeatures = {};
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo deviceCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
  deviceCreateInfo.queueCreateInfoCount = indexCount;
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
  deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
  deviceCreateInfo.enabledExtensionCount = 1;
  const char *extensionNames = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  deviceCreateInfo.ppEnabledExtensionNames = &extensionNames;

  // Deprecated by Vulkan API, so set to 0
  deviceCreateInfo.enabledLayerCount = 0;
  deviceCreateInfo.ppEnabledLayerNames = 0;
  

  VK_CHECK(vkCreateDevice(
        context->device.physicalDevice,
        &deviceCreateInfo,
        context->allocator,
        &context->device.device));
  S_TraceLogInfo("Logical device created successfuly");

  vkGetDeviceQueue(
      context->device.device,
      context->device.graphicsQueueIndex,
      0,
      &context->device.graphicsQueue);

  vkGetDeviceQueue(
      context->device.device,
      context->device.presentQueueIndex,
      0,
      &context->device.presentQueue); 

  vkGetDeviceQueue(
      context->device.device,
      context->device.transferQueueIndex,
      0,
      &context->device.transferQueue);
  S_TraceLogInfo("Queues obtained");

  return TRUE;
}

void VulkanDestroyDevice(VulkanContext *context) {
  context->device.graphicsQueue = 0;
  context->device.presentQueue = 0;
  context->device.transferQueue = 0;

  if (context->device.device) {
    vkDestroyDevice(context->device.device, context->allocator);
    context->device.device = 0;
  }

  context->device.physicalDevice = 0;

  if (context->device.swapchainSupport.presentMode) {
    ufree(
        context->device.swapchainSupport.formats,
        sizeof(VkSurfaceFormatKHR) * context->device.swapchainSupport.formatCount,
        MEMORY_CATEGORY_RENDERER);
    context->device.swapchainSupport.presentMode = 0;
    context->device.swapchainSupport.presentModeCount = 0;
  }

//   ufree(
//       &context->device.swapchainSupport.capabilities,
//       sizeof(context->device.swapchainSupport.capabilities),
//       MEMORY_CATEGORY_RENDERER);

  context->device.graphicsQueueIndex = -1;
  context->device.presentQueueIndex = -1;
  context->device.transferQueueIndex = -1;
}

void VulkanDeviceSwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VulkanSwapchainSupportInfo *outSupportInfo) {
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &outSupportInfo->capabilities));

  VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &outSupportInfo->formatCount, 0));

  if (outSupportInfo->formatCount != 0) {
    if (outSupportInfo->formats)
      outSupportInfo->formats = ualloc(sizeof(VkSurfaceFormatKHR) * outSupportInfo->formatCount, MEMORY_CATEGORY_RENDERER);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &outSupportInfo->formatCount, outSupportInfo->formats));
  }

  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &outSupportInfo->presentModeCount, 0));

  if (outSupportInfo->presentModeCount != 0) {
    if (!outSupportInfo->presentMode)
      outSupportInfo->presentMode = ualloc(sizeof(VkPresentModeKHR) * outSupportInfo->presentModeCount, MEMORY_CATEGORY_RENDERER);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &outSupportInfo->presentModeCount, outSupportInfo->presentMode));
  }
}

BOOLEAN SelectPhysicalDevice(VulkanContext *context) {
  u32 physicalDeviceCount = 0;
  VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physicalDeviceCount, 0));
  if (physicalDeviceCount == 0) {
    S_TraceLogError("No physical device with Vulkan support were fount");
    return FALSE;
  }

  VkPhysicalDevice physicalDevices[physicalDeviceCount];
  VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physicalDeviceCount, physicalDevices));

  for (u32 i = 0; i < physicalDeviceCount; ++i) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physicalDevices[i], &features);

    VkPhysicalDeviceMemoryProperties memory;
    vkGetPhysicalDeviceMemoryProperties(physicalDevices[i], &memory);

    // TODO: These requirements should probably be driven by engine config
    VulkanPhysicalDeviceRequirements requirements = {};
    requirements.graphics = TRUE;
    requirements.present = TRUE;
    requirements.transfer = TRUE;
    requirements.samplerAnisotropy = TRUE;
    requirements.discreteGPU = TRUE;
    requirements.deviceExtensionNames = da_init(const char *);
    da_push(requirements.deviceExtensionNames, &VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    VulkanPhysicalDeviceQueueFamilyInfo queueInfo = {};
    BOOLEAN result = PhysicalDeviceMeetsRequirements(
        physicalDevices[i],
        context->surface,
        &properties,
        &features,
        &requirements,
        &queueInfo,
        &context->device.swapchainSupport);

    if (result) {
      S_TraceLogInfo("Selected device: %s", properties.deviceName);
      switch (properties.deviceType) {
        default:
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
          S_TraceLogInfo("Unknown GPU type");
          break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
          S_TraceLogInfo("GPU type is integrated");
          break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
          S_TraceLogInfo("GPU type is discrete");
          break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
          S_TraceLogInfo("GPU type is virtual");
          break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
          S_TraceLogInfo("GPU is a CPU 0o0");
          break;
      }

      S_TraceLogInfo("GPU Driver version: %d.%d.%d",
          VK_VERSION_MAJOR(properties.driverVersion),
          VK_VERSION_MINOR(properties.driverVersion),
          VK_VERSION_PATCH(properties.driverVersion));

      S_TraceLogInfo("Vulkan API version: %d.%d.%d",
          VK_VERSION_MAJOR(properties.apiVersion),
          VK_VERSION_MINOR(properties.apiVersion),
          VK_VERSION_PATCH(properties.apiVersion));

      // Renderer GPU memory usage
      // TODO: Maybe extract
      for (u32 j = 0; j < memory.memoryHeapCount; ++j) {
          f32 memory_size_gib = (((f32)memory.memoryHeaps[j].size) / 1024.0f / 1024.0f / 1024.0f);
          if (memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
              S_TraceLogInfo("Local GPU memory: %.2f GiB", memory_size_gib);
          else S_TraceLogInfo("Shared System memory: %.2f GiB", memory_size_gib);
      }

      context->device.physicalDevice = physicalDevices[i];
      context->device.graphicsQueueIndex = queueInfo.graphicsFamilyIndex;
      context->device.presentQueueIndex = queueInfo.presentFamilyIndex;
      context->device.transferQueueIndex = queueInfo.transferFamilyIndex;

      context->device.properties = properties;
      context->device.features = features;
      context->device.memory = memory;
      break;
    }
  }

  if (!context->device.physicalDevice) {
    S_TraceLogError("No physical device which meets the requirement were found :(");
    return FALSE;
  }

  return TRUE;
}

BOOLEAN PhysicalDeviceMeetsRequirements(
  VkPhysicalDevice device,
  VkSurfaceKHR surface,
  const VkPhysicalDeviceProperties *properties,
  const VkPhysicalDeviceFeatures *features,
  const VulkanPhysicalDeviceRequirements *requirements,
  VulkanPhysicalDeviceQueueFamilyInfo *outQueueFamilyInfo,
  VulkanSwapchainSupportInfo *outSwapchainSupport)
{
  // All families = -1
  outQueueFamilyInfo->graphicsFamilyIndex = -1;
  outQueueFamilyInfo->presentFamilyIndex = -1;
  outQueueFamilyInfo->computeFamilyIndex = -1;
  outQueueFamilyInfo->transferFamilyIndex = -1;

  if (requirements->discreteGPU) {
    if (properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      S_TraceLogWarn("Device is not a discrete GPU, skipping");
      return FALSE;
    }
  }

  u32 outQueueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &outQueueFamilyCount, 0);
  VkQueueFamilyProperties queueFamilies[outQueueFamilyCount];
  vkGetPhysicalDeviceQueueFamilyProperties(device, &outQueueFamilyCount, queueFamilies);

  S_TraceLogInfo("Graphics | Present | Compute | Transfer | Name");
  u8 minTransferScore = 255;
  for (u32 i = 0; i < outQueueFamilyCount; ++i) {
    u8 currentTransferScore = 0;

    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      outQueueFamilyInfo->graphicsFamilyIndex = i;
      ++currentTransferScore;
    }

    if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
      outQueueFamilyInfo->computeFamilyIndex = i;
      ++currentTransferScore;
    }

    if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
      if (currentTransferScore <= minTransferScore) {
        minTransferScore = currentTransferScore;
        outQueueFamilyInfo->transferFamilyIndex = i;
      }
    }

    VkBool32 supportsPresent = VK_FALSE;
    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supportsPresent));
    if (supportsPresent)
      outQueueFamilyInfo->presentFamilyIndex = i;
  }

  S_TraceLogInfo("       %d |       %d |       %d |        %d | %s",
      outQueueFamilyInfo->graphicsFamilyIndex != -1,
      outQueueFamilyInfo->presentFamilyIndex != -1,
      outQueueFamilyInfo->computeFamilyIndex != -1,
      outQueueFamilyInfo->transferFamilyIndex != -1,
      properties->deviceName);
  
  if (
      (!requirements->graphics || (requirements->graphics && outQueueFamilyInfo->graphicsFamilyIndex != -1)) &&
      (!requirements->present || (requirements->present && outQueueFamilyInfo->presentFamilyIndex != -1)) &&
      (!requirements->compute || (requirements->compute && outQueueFamilyInfo->computeFamilyIndex != -1)) &&
      (!requirements->transfer || (requirements->transfer && outQueueFamilyInfo->transferFamilyIndex != -1))) 
  {
    S_TraceLogInfo("Device meets queue requirements");

    // TODO: Change to S_TraceLog (Because Trace)
    S_TraceLogInfo("Graphics Family Index: %i", outQueueFamilyInfo->graphicsFamilyIndex);
    S_TraceLogInfo("Present Family Index:  %i", outQueueFamilyInfo->presentFamilyIndex);
    S_TraceLogInfo("Transfer Family Index: %i", outQueueFamilyInfo->transferFamilyIndex);
    S_TraceLogInfo("Compute Family Index:  %i", outQueueFamilyInfo->computeFamilyIndex);

    VulkanDeviceSwapchainSupport(device, surface, outSwapchainSupport);

    if (outSwapchainSupport->formatCount < 1 || outSwapchainSupport->presentModeCount < 1) {
      if (outSwapchainSupport->formats)
        ufree(outSwapchainSupport->formats, sizeof(VkSurfaceFormatKHR) * outSwapchainSupport->formatCount, MEMORY_CATEGORY_RENDERER);
      if (outSwapchainSupport->presentMode)
        ufree(outSwapchainSupport->presentMode, sizeof(VkPresentModeKHR) * outSwapchainSupport->presentModeCount, MEMORY_CATEGORY_RENDERER);
      S_TraceLogInfo("Required swapchain not present, skipping device");
      return FALSE;
    }
  
    if (requirements->deviceExtensionNames) {
      u32 availableExtensionCount = 0;
      VkExtensionProperties *availableExtensions = 0;
      VK_CHECK(vkEnumerateDeviceExtensionProperties(device, 0, &availableExtensionCount, 0));
      if (availableExtensionCount != 0) {
        availableExtensions = ualloc(sizeof(VkExtensionProperties) * availableExtensionCount, MEMORY_CATEGORY_RENDERER);
        VK_CHECK(vkEnumerateDeviceExtensionProperties(device, 0, &availableExtensionCount, availableExtensions));

        u32 requiredExtensionCount = da_length(requirements->deviceExtensionNames);
        for (u32 i = 0; i < requiredExtensionCount; ++i) {
          BOOLEAN found = FALSE;
          for (u32 j = 0; j < availableExtensionCount; ++j) {
            if (ustrcmp(requirements->deviceExtensionNames[i], availableExtensions[j].extensionName)) {
              found = TRUE;
              break;
            }
          }

          if (!found) {
            S_TraceLogWarn("Require extension '%s' not found, skipping device", requirements->deviceExtensionNames[i]);
            return FALSE;
          }
        }
      }
      ufree(availableExtensions, sizeof(VkExtensionProperties) * availableExtensionCount, MEMORY_CATEGORY_RENDERER);
    }

    if (requirements->samplerAnisotropy && !requirements->samplerAnisotropy) {
      S_TraceLogWarn("Device doesn't support sampler anisotropy, skipping");
      return FALSE;
    }

    return TRUE;
  }

  return FALSE;
}
