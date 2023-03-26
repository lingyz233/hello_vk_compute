#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<vulkan/vulkan.h>

_Bool
check_layer_support (const char * const layer_name)
{
  static uint32_t layer_cnt;
  static VkLayerProperties * available_layers;

  if (!layer_name) // get available layers
  {
    if (!available_layers)
      free (available_layers);
    vkEnumerateInstanceLayerProperties (&layer_cnt, NULL);
    available_layers = malloc (sizeof (VkLayerProperties) * layer_cnt);
    vkEnumerateInstanceLayerProperties (&layer_cnt, available_layers);
    return 0;
  }

  if (layer_name == (void*)-1) // print all available layers
  {
    for (int i = 0; i < layer_cnt; i++)
      printf ("%s\n", available_layers[i].layerName);
    return 0;
  }

  for (int i = 0; i < layer_cnt; i++) // check layer
    if (strcmp (layer_name, available_layers[i].layerName) == 0)
      return 1;
  return 0;
}

