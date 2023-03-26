#ifndef __INSTANCE_H__
#define __INSTANCE_H__

#include<vulkan/vulkan.h>

static void
getextensions (VkInstanceCreateInfo * createinfo);

VkInstance
create_instance (char * name);

void
destroy_instance (VkInstance instance);

#endif
