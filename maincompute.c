#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<err.h>

#include"compute.h"
#include"vkbuffer.h"
#include"instance.h"
#include"devices.h"

#include"filemap.h"

#define CNT 1024

int
main (void)
{
  VkInstance instance = create_instance ("first_app");
  VkPhysicalDevice phydev;
  VkDevice device;
  VkQueue queue;
  int qfam;
  device = create_device_and_queue (instance, &phydev, &queue, &qfam);

  VkBuffer buf;
  buf = prepare_buffer (device, CNT*sizeof(float));
  VkDeviceMemory devmem;
  devmem = alloc_bind_memory (device, phydev, buf);
//start input data ...
  float * data = malloc (sizeof (float) * CNT);
  for (int i = 0; i < CNT; i++)
    data[i] = i;

  float * mapaddr;
  if (vkMapMemory (device, devmem, 0, sizeof (float) * CNT, 0, (void**)&mapaddr) != VK_SUCCESS)
    errx (-1, "(%s) failed to map memory\n", __FUNCTION__);
  memcpy (mapaddr, data, sizeof (float) * CNT);
  vkUnmapMemory (device, devmem);
//end input data to ..,
  VkDescriptorSetLayout desc_set_layout;
  desc_set_layout = create_descriptor_set_layout (device, 1, (uint32_t[1]){0});

  VkDescriptorSet desc_set;
  desc_set = create_descriptor_set (device, &desc_set_layout);

  add_buffer_to_descriptor_set (device, desc_set, 1, (VkBuffer[1]){buf}, (size_t[1]){sizeof (float) * CNT} , (int[1]){0});

  VkCommandPool commandpool;
  commandpool = create_command_pool (device, qfam);

  VkShaderModule shader;
  {
    struct filemap compshadercode;
    filemap.open (&compshadercode, "./double.comp.spv", O_RDONLY, MAP_SHARED, PROT_READ, 0, 0);
    shader = create_shader_module (device, compshadercode.d, compshadercode.fsize);
    filemap.close (&compshadercode);
  }

  VkPipeline pipeline;
  VkPipelineLayout ppl_lay;
  pipeline = create_compute_pipeline (device, shader, 1, (VkDescriptorSetLayout[1]){desc_set_layout}, &ppl_lay);

  VkCommandBuffer cmdbuf;
  cmdbuf = create_command_buffer (device, commandpool);

  record_command_buffer (cmdbuf, ppl_lay, pipeline, 0, 1, &desc_set, (int[3]){CNT/256,1,1});

  sleep (1);
  if (vkQueueWaitIdle (queue) != VK_SUCCESS)
    errx (-1, "vkQueueWaitIdle\n");
  sleep (1);
//start print ...
//
  memset (data, 0, sizeof (float) * CNT);
  if (vkMapMemory (device, devmem, 0, sizeof (float) * CNT, 0, (void**)&mapaddr) != VK_SUCCESS)
    errx (-1, "(%s) failed to map memory\n", __FUNCTION__);
//  memcpy (data, mapaddr, sizeof (float) * 1024);

  for (int i = 0; i < CNT; i++)
    printf ("%.1f \t", mapaddr[i]);

  vkUnmapMemory (device, devmem);
//end print ...
  putchar (10);

  return 0;
}
