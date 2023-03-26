#include<stdio.h>
#include<vulkan/vulkan.h>

static uint32_t find_memory_type (uint32_t tp_filter, VkMemoryPropertyFlags properties,VkPhysicalDevice phy_dev);

VkBuffer
prepare_buffer (VkDevice dev, size_t datasize)
{
  VkBuffer buf;
  {
    VkBufferCreateInfo bci = {0};
    bci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bci.size = datasize;
    bci.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bci.queueFamilyIndexCount = 0;
    bci.pQueueFamilyIndices = NULL;

    if (vkCreateBuffer (dev, &bci, NULL, &buf) != VK_SUCCESS)
    {
      fprintf (stderr, "failed to create vk buffer\n");
      return NULL;
    }
  }

  return buf;
}

VkDeviceMemory
alloc_bind_memory (VkDevice dev, VkPhysicalDevice phy_dev, VkBuffer buf)
{
  VkDeviceMemory dev_mem;
  {
    VkMemoryRequirements mr;
    vkGetBufferMemoryRequirements (dev, buf, &mr);

    VkMemoryAllocateInfo mai = {0};
    mai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mai.allocationSize = mr.size;
    mai.memoryTypeIndex = find_memory_type (mr.memoryTypeBits, 
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, phy_dev);

    if (vkAllocateMemory (dev, &mai, NULL, &dev_mem) != VK_SUCCESS)
    {
      fprintf (stderr, "failed to create device memory\n");
      return NULL;
    }
  }

  vkBindBufferMemory (dev, buf, dev_mem, 0);
  //shoude be freed at the end
  
  return dev_mem;
}

static uint32_t find_memory_type (uint32_t tp_filter, VkMemoryPropertyFlags properties,VkPhysicalDevice phy_dev)
{
  VkPhysicalDeviceMemoryProperties pdmp;
  vkGetPhysicalDeviceMemoryProperties (phy_dev, &pdmp);

  for (int i = 0; i < pdmp.memoryTypeCount; i++)
    if (tp_filter & (1<<i) && (pdmp.memoryTypes[i].propertyFlags & properties)==properties)
	  return i;
  return -1;
}
