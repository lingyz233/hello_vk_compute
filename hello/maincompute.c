#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<err.h>

#include"compute.h"
#include"vkbuffer.h"
#include"instance.h"
#include"devices.h"

#include"filemap.h"

#define CNT (1024)

int
main (void)
{
  VkInstance instance = create_instance ("first_app");
  VkPhysicalDevice phydev;
  VkDevice device;
  VkQueue queue;
  uint32_t qfam;
  device = create_device_and_queue (instance, &phydev, &queue, &qfam);

  VkBuffer buf;
  buf = prepare_buffer (device, CNT*sizeof(float));
  VkDeviceMemory devmem;
  devmem = alloc_bind_memory (device, phydev, buf);

  float * data = malloc (sizeof (float) * CNT);
  for (int i = 0; i < CNT; i++)
    data[i] = i / 12.;

  float * mapaddr;
  if (vkMapMemory (device, devmem, 0, sizeof (float) * CNT, 0, (void**)&mapaddr) != VK_SUCCESS)
    errx (-1, "(%s) failed to map memory\n", __FUNCTION__);
  memcpy (mapaddr, data, sizeof (float) * CNT);

  vkUnmapMemory (device, devmem);

  VkDescriptorSetLayout desc_set_layout;
  desc_set_layout = create_descriptor_set_layout (device, 1, (uint32_t[1]){0});

  VkDescriptorPool pool;
  pool = create_descriptor_pool (device, 1);
  VkDescriptorSet desc_set;
  desc_set = create_descriptor_set (device, pool, &desc_set_layout);
  descriptor_set_bind (device, desc_set, buf, sizeof (float) * CNT, 0);

  VkCommandPool commandpool;
  commandpool = create_command_pool (device, qfam);

  VkPipeline pipeline;
  VkPipelineLayout ppl_lay;
  {
    VkShaderModule shader;
    {
      struct filemap compshadercode;
      filemap.open (&compshadercode, "./myexp.comp.spv", O_RDONLY, MAP_SHARED, PROT_READ, 0, 0);
      shader = create_shader_module (device, compshadercode.d, compshadercode.fsize);
      filemap.close (&compshadercode);
    }

    pipeline = create_compute_pipeline (device, shader, 1, (VkDescriptorSetLayout[1]){desc_set_layout}, &ppl_lay);
    vkDestroyShaderModule (device, shader, NULL);
  }

  VkCommandBuffer cmdbuf;
  cmdbuf = create_command_buffer (device, commandpool);

  record_command_buffer (cmdbuf, ppl_lay, pipeline, 0, 1, &desc_set, (int[3]){CNT/256,1,1});


  {
    VkSubmitInfo si = {};
    si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    si.commandBufferCount = 1;
    si.pCommandBuffers = &cmdbuf;
    si.waitSemaphoreCount = 0;
    si.signalSemaphoreCount = 0;
//    for (;;)
    if (vkQueueSubmit (queue, 1, &si, VK_NULL_HANDLE) != VK_SUCCESS)
      errx (-1, "failed to submit command buffer\n");
  }

  if (vkQueueWaitIdle (queue) != VK_SUCCESS)
    errx (-1, "vkQueueWaitIdle\n");


  if (vkMapMemory (device, devmem, 0, sizeof (float) * CNT, 0, (void**)&mapaddr) != VK_SUCCESS)
    errx (-1, "(%s) failed to map memory\n", __FUNCTION__);
//  memcpy (data, mapaddr, sizeof (float) * 1024);

  for (int i = 0; i < CNT; i++)
  {
    printf ("%.5e \t", mapaddr[i]);
	if (i % 4 == 3)
	  putchar (10);
	if (i % 12 == 11)
	  putchar (10);
  }

  vkUnmapMemory (device, devmem);

  putchar (10);


  vkDestroyBuffer (device, buf, NULL);
  vkFreeMemory (device, devmem, NULL);
  vkDestroyPipeline (device, pipeline, NULL);
  vkDestroyPipelineLayout (device, ppl_lay, NULL);

  vkFreeCommandBuffers (device, commandpool, 1, &cmdbuf);
  vkDestroyCommandPool (device, commandpool, NULL);

  vkDestroyDescriptorPool (device, pool, NULL);

//  vkDestroyDevice (device, NULL);

//  vkDestroyInstance (instance, NULL);


  return 0;
}
