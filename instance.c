#include<vulkan/vulkan.h>

#include<stdio.h>
#include<err.h>
#include<stdlib.h>

#include"debug.h"
#include"checklayers.h"

static void
getextensions (VkInstanceCreateInfo * createinfo)
{
  if (1) //use vkEnumerateInstanceExtensionProperties to set extensions
  {
    uint32_t extensioncount = 0;
	VkExtensionProperties * extensions;

    vkEnumerateInstanceExtensionProperties (NULL, &extensioncount, NULL);
    extensions = malloc (sizeof(VkExtensionProperties) * extensioncount);
    vkEnumerateInstanceExtensionProperties (NULL, &extensioncount, extensions);

    char ** extnames = malloc (sizeof(void*) * extensioncount);
    for (int i = 0; i < extensioncount; i++)
    {
      extnames[i] = extensions[i].extensionName;
//      printf ("%s %d\n", extensions[i].extensionName, extensions[i].specVersion);
    }

    createinfo->enabledExtensionCount = extensioncount;
    createinfo->ppEnabledExtensionNames = (char const*const*)extnames;
  }
}

VkInstance
create_instance ()
{
  VkInstance instance;
  VkApplicationInfo appInfo = {}; //optional
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pNext = NULL;
  appInfo.pApplicationName = "Hello Triangle";
  appInfo.applicationVersion = VK_MAKE_VERSION (1,3,0);
  appInfo.pEngineName = NULL;
  appInfo.engineVersion = VK_MAKE_VERSION (1,3,0);
  appInfo.apiVersion = VK_API_VERSION_1_3;

  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  getextensions (&createInfo);

  createInfo.enabledLayerCount = 0;

  if (__enable_validation_layer__)
  {
    check_layer_support (NULL);
    check_layer_support ((void*)-1);
    if (check_layer_support ("VK_LAYER_KHRONOS_validation"))
    {
      fprintf (stderr, "using validation layer\n");
//      createInfo.enabledLayerCount = 2;
//      createInfo.ppEnabledLayerNames = (const char*[]){"VK_LAYER_KHRONOS_validation","VK_LAYER_LUNARG_api_dump"};
      createInfo.enabledLayerCount = 1;
      createInfo.ppEnabledLayerNames = (const char*[]){"VK_LAYER_KHRONOS_validation"};
      VkValidationFeaturesEXT vfext = {};
      vfext.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
      vfext.enabledValidationFeatureCount = 1;
      vfext.pEnabledValidationFeatures = (VkValidationFeatureEnableEXT[]){VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT};
      vfext.disabledValidationFeatureCount = 0;
      vfext.pDisabledValidationFeatures = NULL;
	  createInfo.pNext = &vfext;
    }
    else
    {
      fprintf (stderr, "validation layer required but not availible !\n");
    }
  }

  int result = vkCreateInstance (&createInfo, NULL, &instance);
  if (result != VK_SUCCESS)
    errx (-1, "failed to create vk instance : result : %d\n", result);
  else
    fprintf (stderr, "succesfully creating vk instance !\n");
  return instance;
}

void
destroy_instance (VkInstance instance)
{
  vkDestroyInstance (instance, NULL);
}
