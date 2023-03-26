#ifndef __VKBUFFER_H__
#define __VKBUFFER_H__

#include<vulkan/vulkan.h>

VkBuffer
prepare_buffer (VkDevice dev, size_t datasize);

VkDeviceMemory
alloc_bind_memory (VkDevice dev, VkPhysicalDevice phy_dev, VkBuffer buf);
#endif
