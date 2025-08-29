> [!WARNING]
> Development will be freezed for a while soon

# Ücima Game Engine
Fully 3D (maybe 2D) Game engine with multiple graphical API support, such as Vulkan, OpenGL, DirectX11, Metal.

### In plans:
 - Implement lua support as a game programming language. And give an ability to create native games, but programmed in c.

> Current object: Fix segfault.
> Logs:
```
[INFO]: Memory subsystem initialized successfuly
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
[INFO]: Input subsystem initialized successfuly
[INFO]: Event subsystem initialized successfuly
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: IMPORTANT TODO: Use dynamic arrays (File: ./src/renderer/vulkan/vulkan_backend.c, Line: 35)
message: TODO: Custom vulkan allocator (File: ./src/renderer/vulkan/vulkan_backend.c, Line: 39)
message: ULTRA FUCKING BIG KOSTIL HERE BLYAT. FIX THIS, SUKA OR DIIIIIIIIIIIEEEEEEEEEEEEE (File: ./src/renderer/vulkan/vulkan_backend.c, Line: 63)
[DEBUG]: Required extensions:
[DEBUG]: VK_KHR_surface
[DEBUG]: VK_KHR_xcb_surface
[DEBUG]: VK_EXT_debug_utils
message: ETO PROSTO PIZDEC BLYAT (File: ./src/renderer/vulkan/vulkan_backend.c, Line: 92)
[DEBUG]: Validation layers enabled. Enumerating...
[DEBUG]: Searching for layer: VK_LAYER_KHRONOS_validation...
[DEBUG]: Found
[DEBUG]: All required validation layers are present
[DEBUG]: Vulkan instance created successfully
message: TODO: Vulkan debugger (#014) (File: ./src/renderer/vulkan/vulkan_backend.c, Line: 136)
[DEBUG]: Creating vulkan surface
[DEBUG]: Vulkan surface created successfully
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
[INFO]: Graphics | Present | Compute | Transfer | Name
[INFO]:        1 |       1 |       1 |        1 | AMD Radeon RX 580 Series (RADV POLARIS10)
[INFO]: Device meets queue requirements
[INFO]: Graphics Family Index: 0
[INFO]: Present Family Index:  1
[INFO]: Transfer Family Index: 1
[INFO]: Compute Family Index:  1
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 72)
[INFO]: Selected device: AMD Radeon RX 580 Series (RADV POLARIS10)
[INFO]: GPU type is discrete
[INFO]: GPU Driver version: 25.2.1
[INFO]: Vulkan API version: 1.4.318
[INFO]: Local GPU memory: 7.75 GiB
[INFO]: Shared System memory: 6.77 GiB
[INFO]: Local GPU memory: 0.25 GiB
[INFO]: Creating logical device...
Validation Error: [ VUID-VkDeviceQueueCreateInfo-pQueuePriorities-00383 ] | MessageID = 0x55c518a0
vkCreateDevice(): pCreateInfo->pQueueCreateInfos[0].pQueuePriorities[1] (724794550188476316886641934336.000000) is not between 0 and 1 (inclusive).
The Vulkan spec states: Each element of pQueuePriorities must be between 0.0 and 1.0 inclusive (https://docs.vulkan.org/spec/latest/chapters/devsandqueues.html#VUID-VkDeviceQueueCreateInfo-pQueuePriorities-00383)
Objects: 1
    [0] VkPhysicalDevice 0x55f170c9f7b0

Validation Error: [ VUID-VkDeviceQueueCreateInfo-queueCount-00382 ] | MessageID = 0x6a8ef7de
vkCreateDevice(): pCreateInfo->pQueueCreateInfos[0].queueCount (2) is not less than or equal to available queue count for this pCreateInfo->pQueueCreateInfos[0].queueFamilyIndex (0) obtained previously from vkGetPhysicalDeviceQueueFamilyProperties or vkGetPhysicalDeviceQueueFamilyProperties2[KHR] (needs to be less than or equal to 1).
The Vulkan spec states: queueCount must be less than or equal to the queueCount member of the VkQueueFamilyProperties structure, as returned by vkGetPhysicalDeviceQueueFamilyProperties in the pQueueFamilyProperties[queueFamilyIndex] (https://docs.vulkan.org/spec/latest/chapters/devsandqueues.html#VUID-VkDeviceQueueCreateInfo-queueCount-00382)
Objects: 1
    [0] VkPhysicalDevice 0x55f170c9f7b0

[INFO]: Logical device created successfuly
[INFO]: Queues obtained
[INFO]: Graphics command pool created successfuly
[DEBUG]: Vulkan logical device created successfully
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
[INFO]: Vulakn swapchain created successfuly
[DEBUG]: Vulkan swapchain created successfully
[DEBUG]: Vulkan renderpass created successfully
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
[DEBUG]: Vulkan framebuffers created successfully
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
[DEBUG]: Vulkan buffers created successfully
[DEBUG]: Creating sync objects...
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
[DEBUG]: Vulkan renderer initialized successfully
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
[INFO]: Engine categorized memory usage:
	UNKNOWN: 0.00B
	ARRAY: 0.00B
	DARRAY: 728.00B
	DICT: 0.00B
	RING_QUEUE: 0.00B
	BST: 0.00B
	STRING: 0.00B
	APPLICATION: 0.00B
	JOB: 0.00B
	TEXTURE: 0.00B
	MAT_INST: 0.00B
	RENDERER: 216.00B
	GAME: 4.00B
	TRANSFRORM: 0.00B
	ENTITY: 0.00B
	ENTITY_NONE: 0.00B
	SCENE: 0.00B

[DEBUG]: Window resized: 900, 800
[DEBUG]: Vulkan renderer backend->resize (w/h/gen): 900/800/1
[DEBUG]: Window resized: 958, 1042
[DEBUG]: Vulkan renderer backend->resize (w/h/gen): 958/1042/2
[INFO]: Vulakn swapchain created successfuly
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 72)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 72)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 72)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 72)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 58)
[DEBUG]: Resized, booting
Validation Error: [ VUID-vkQueueSubmit-pSignalSemaphores-00067 ] | MessageID = 0x539277af
vkQueueSubmit(): pSubmits[0].pSignalSemaphores[0] (VkSemaphore 0x150000000015) is being signaled by VkQueue 0x55f170d3bdd0, but it may still be in use by VkSwapchainKHR 0x1a000000001a.
Here are the most recently acquired image indices: [0], 1, 2.
(brackets mark the last use of VkSemaphore 0x150000000015 in a presentation operation)
Swapchain image 0 was presented but was not re-acquired, so VkSemaphore 0x150000000015 may still be in use and cannot be safely reused with image index 2.
Vulkan insight: One solution is to assign each image its own semaphore. Here are some common methods to ensure that a semaphore passed to vkQueuePresentKHR is not in use and can be safely reused:
	a) Use a separate semaphore per swapchain image. Index these semaphores using the index of the acquired image.
	b) Consider the VK_KHR_swapchain_maintenance1 extension. It allows using a VkFence with the presentation operation.
The Vulkan spec states: Each binary semaphore element of the pSignalSemaphores member of any element of pSubmits must be unsignaled when the semaphore signal operation it defines is executed on the device (https://docs.vulkan.org/spec/latest/chapters/cmdbuffers.html#VUID-vkQueueSubmit-pSignalSemaphores-00067)
Objects: 2
    [0] VkSemaphore 0x150000000015
    [1] VkQueue 0x55f170d3bdd0

Validation Error: [ VUID-vkQueueSubmit-pSignalSemaphores-00067 ] | MessageID = 0x539277af
vkQueueSubmit(): pSubmits[0].pSignalSemaphores[0] (VkSemaphore 0x180000000018) is being signaled by VkQueue 0x55f170d3bdd0, but it may still be in use by VkSwapchainKHR 0x1a000000001a.
Here are the most recently acquired image indices: 0, [1], 2, 3.
(brackets mark the last use of VkSemaphore 0x180000000018 in a presentation operation)
Swapchain image 1 was presented but was not re-acquired, so VkSemaphore 0x180000000018 may still be in use and cannot be safely reused with image index 3.
Vulkan insight: One solution is to assign each image its own semaphore. Here are some common methods to ensure that a semaphore passed to vkQueuePresentKHR is not in use and can be safely reused:
	a) Use a separate semaphore per swapchain image. Index these semaphores using the index of the acquired image.
	b) Consider the VK_KHR_swapchain_maintenance1 extension. It allows using a VkFence with the presentation operation.
The Vulkan spec states: Each binary semaphore element of the pSignalSemaphores member of any element of pSubmits must be unsignaled when the semaphore signal operation it defines is executed on the device (https://docs.vulkan.org/spec/latest/chapters/cmdbuffers.html#VUID-vkQueueSubmit-pSignalSemaphores-00067)
Objects: 2
    [0] VkSemaphore 0x180000000018
    [1] VkQueue 0x55f170d3bdd0

message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 72)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 72)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 72)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 72)
message: TODO: Destroy semaphores properly (File: ./src/renderer/vulkan/vulkan_backend.c, Line: 203)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 72)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 72)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 72)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 72)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 72)
Validation Error: [ VUID-vkDestroyDevice-device-05137 ] | MessageID = 0x4872eaa0
vkDestroyDevice(): Object Tracking - For VkDevice 0x55f170d2bc70, VkSemaphore 0x140000000014 has not been destroyed.
The Vulkan spec states: All child objects created on device that can be destroyed or freed must have been destroyed or freed prior to destroying device (https://docs.vulkan.org/spec/latest/chapters/devsandqueues.html#VUID-vkDestroyDevice-device-05137)
Objects: 1
    [0] VkSemaphore 0x140000000014

Validation Error: [ VUID-vkDestroyDevice-device-05137 ] | MessageID = 0x4872eaa0
vkDestroyDevice(): Object Tracking - For VkDevice 0x55f170d2bc70, VkSemaphore 0x170000000017 has not been destroyed.
The Vulkan spec states: All child objects created on device that can be destroyed or freed must have been destroyed or freed prior to destroying device (https://docs.vulkan.org/spec/latest/chapters/devsandqueues.html#VUID-vkDestroyDevice-device-05137)
Objects: 1
    [0] VkSemaphore 0x170000000017

message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 72)
message: TODO: Memory alignment (File: ./src/core/umemory.c, Line: 72)
message: TODO: DestroyMemory() (File: ./src/core/umemory.c, Line: 47)
```
