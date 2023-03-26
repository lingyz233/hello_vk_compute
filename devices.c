#include<stdio.h>
#include<stdlib.h>

#include<vulkan/vulkan.h>

#include"debug.h"

typedef struct queue_family_indices {
  uint32_t graphics;
  uint32_t compute;
} queue_family_indices;

_Bool
check_physical_device (VkPhysicalDevice device, _Bool additional);

queue_family_indices
find_queue_families (VkPhysicalDevice device);

VkPhysicalDevice
pick_physical_device (VkInstance instance);

_Bool
destroy_device (VkDevice device)
{
  vkDestroyDevice (device, NULL);
  return 0;
}

VkDevice
create_device_and_queue (VkInstance instance, VkPhysicalDevice * pphy_dev, VkQueue * pqueue, uint32_t * queue_family_idx)
{
  VkPhysicalDevice phy_dev = pick_physical_device (instance);
  *pphy_dev = phy_dev;

  if (phy_dev == VK_NULL_HANDLE)
  {
    fprintf (stderr, "failed to get a availible physical device\n");
  }

  VkDeviceQueueCreateInfo queue_ci = {};
  queue_ci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_ci.queueFamilyIndex = (find_queue_families (phy_dev)).compute;
//  queue_ci.queueFamilyIndex = (find_queue_families (phy_dev)).graphics;
  queue_ci.queueCount = 1;
  float queue_priority = 0.9f;
  queue_ci.pQueuePriorities = &queue_priority;

  VkPhysicalDeviceFeatures dev_features = {0};

  VkPhysicalDeviceProperties props; 
  vkGetPhysicalDeviceProperties(phy_dev, &props);
  printf("pick device %s (v:0x%x, d:0x%x)\n",
    props.deviceName, props.vendorID, props.deviceID);


  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.pQueueCreateInfos = &queue_ci;
  create_info.queueCreateInfoCount = 1;
  create_info.pEnabledFeatures = &dev_features;
  create_info.enabledExtensionCount = 0;

  if (__enable_validation_layer__)
  {
    create_info.enabledLayerCount = 1;
    create_info.ppEnabledLayerNames = &validation_layer_name;
//    create_info.enabledLayerCount = 2;
//    create_info.ppEnabledLayerNames = (const char*const*){validation_layer_name,"VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME"};
	fprintf (stderr, "using validation layer to create logical device\n");
  }
  else
  {
    create_info.enabledLayerCount = 0;
  }

  if (__enable_validation_layer__)
    printf ("using queue family %d\n", create_info.pQueueCreateInfos->queueFamilyIndex);

  VkDevice device;
  if (vkCreateDevice (phy_dev, &create_info, NULL, &device) != VK_SUCCESS)
  {
    fprintf (stderr, "failed to create logical device\n");
	return VK_NULL_HANDLE;
  }
  else
  {
    fprintf (stderr, "successfully creating logical device\n");
  }

  vkGetDeviceQueue (device, queue_ci.queueFamilyIndex, 0, pqueue);

  if (queue_family_idx)
    *queue_family_idx = queue_ci.queueFamilyIndex;

  return device;
}

VkPhysicalDevice
pick_physical_device (VkInstance instance)
{
  VkPhysicalDevice * phy_dev;
  uint32_t dev_count = 0;
  vkEnumeratePhysicalDevices (instance, &dev_count, NULL);

  if (!dev_count)
  {
    fprintf (stderr, "failed to find GPUs with vulkan support !\n");
    return (VkPhysicalDevice)-1;
  }

  phy_dev = malloc (sizeof (VkPhysicalDevice) * (1+dev_count));
  vkEnumeratePhysicalDevices (instance, &dev_count, phy_dev);
  phy_dev[dev_count] = VK_NULL_HANDLE;
 
  int i = 0;
  while (!check_physical_device (phy_dev[i], 1)
    && (find_queue_families (phy_dev[i])).compute != -1
	&& (find_queue_families (phy_dev[i])).graphics != -1 && i < dev_count )
    ++i;

  VkPhysicalDevice phydev = phy_dev[i];
  free (phy_dev);

  return phydev;
} 

_Bool
check_physical_device (VkPhysicalDevice device, _Bool additional)
{
  VkPhysicalDeviceProperties device_properties;
  vkGetPhysicalDeviceProperties (device, &device_properties);

  VkPhysicalDeviceFeatures device_features;
  vkGetPhysicalDeviceFeatures (device, &device_features);

  return device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && device_features.geometryShader;
}

queue_family_indices
find_queue_families (VkPhysicalDevice device)
{
  queue_family_indices indices = {-1,-1};
  VkQueueFamilyProperties * queue_families;
  uint32_t count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties (device, &count, NULL);
  queue_families = malloc (sizeof (VkQueueFamilyProperties) * (1+count));
  vkGetPhysicalDeviceQueueFamilyProperties (device, &count, queue_families);

  FILE * out;

  if (__enable_validation_layer__)
    out = stdout;
  else
    out = fopen ("/dev/null", "w");

  for (int i = 0; i < count; i++)
  {
    fprintf (out, "for queue family index %d\n", i);
    if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
	{
	  indices.graphics = i;
	  fprintf(out, "%d VK_QUEUE_GRAPHICS_BIT\n", i);
	}
    if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
	{
	  indices.compute = i;
	  fprintf(out, "%d VK_QUEUE_COMPUTE_BIT\n", i);
	}
    if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
	  fprintf(out, "%d VK_QUEUE_TRANSFER_BIT\n", i);
    if (queue_families[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
	  fprintf(out, "%d VK_QUEUE_SPARSE_BINDING_BIT\n", i);
    if (queue_families[i].queueFlags & VK_QUEUE_PROTECTED_BIT)
	  fprintf(out, "%d VK_QUEUE_PROTECTED_BIT\n", i);
    if (queue_families[i].queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR)
	  fprintf(out, "%d VK_QUEUE_VIDEO_DECODE_BIT_KHR\n", i);
#if defined (VK_ENABLE_BETA_EXTENSIONS)
    if (queue_families[i].queueFlags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR)
	  fprintf(out, "%d VK_QUEUE_VIDEO_ENCODE_BIT_KHR\n", i);
#endif
    if (queue_families[i].queueFlags & VK_QUEUE_OPTICAL_FLOW_BIT_NV)
	  fprintf(out, "%d VK_QUEUE_OPTICAL_FLOW_BIT_NV\n", i);
  }

  free (queue_families);

  return indices;
}
