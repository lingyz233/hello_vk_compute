#ifndef __DEVICES_H__
#define __DEVICES_H__

#include<vulkan/vulkan.h>

typedef struct queue_family_indices {
  uint32_t graphics;
  uint32_t compute;
} queue_family_indices;

_Bool
check_physical_device (VkPhysicalDevice device, _Bool additional);

VkDevice
create_device_and_queue (VkInstance instance, VkPhysicalDevice * pphy_dev, VkQueue * pqueue, uint32_t*);

queue_family_indices
find_queue_families (VkPhysicalDevice device);

VkPhysicalDevice
pick_physical_device (VkInstance instance);

_Bool
destroy_device (VkDevice device);

#endif
