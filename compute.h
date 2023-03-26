#ifndef __COMPUTE_H__
#define __COMPUTE_H__

#include<vulkan/vulkan.h>

VkShaderModule
create_shader_module (VkDevice device, char * pcode, size_t size);

void
destroy_shader_module(VkDevice device, VkShaderModule shader_module);

VkPipeline
create_compute_pipeline (VkDevice dev, VkShaderModule shader, int sl_count, VkDescriptorSetLayout ds_layout[sl_count], VkPipelineLayout *);

VkDescriptorSetLayout
create_descriptor_set_layout (VkDevice dev, int binding_count, uint32_t binding[binding_count]);

VkDescriptorSet
create_descriptor_set (VkDevice dev, VkDescriptorSetLayout * pds_layout);

void
add_buffer_to_descriptor_set (VkDevice dev, VkDescriptorSet dstset, int n, VkBuffer buf[n], size_t bufsize[n], int binding[n]);

VkCommandPool
create_command_pool (VkDevice dev, uint32_t queue_fam_idx);

VkCommandBuffer
create_command_buffer (VkDevice dev, VkCommandPool cmd_pool);

void
record_command_buffer (VkCommandBuffer cmd_buf,
  VkPipelineLayout ppl_layout, VkPipeline pipeline, int first_set,
  uint32_t set_cnt, VkDescriptorSet * pds, int dispatch[3]);

#endif
