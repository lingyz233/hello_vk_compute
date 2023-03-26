#include<err.h>
#include<stdio.h>

#include<vulkan/vulkan.h>

VkShaderModule
create_shader_module (VkDevice device, char * pcode, size_t size)
{
  VkShaderModuleCreateInfo smci = {};
  smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  smci.codeSize = size;
  smci.pCode = (uint32_t*)pcode;

  VkShaderModule shader_module;
  if (vkCreateShaderModule (device, &smci, NULL, &shader_module) != VK_SUCCESS)
    errx (-1, "(my)failed to create shader module\n");
  else
    warnx ("(my) shader module created");

  return shader_module;
}

void
destroy_shader_module(VkDevice device, VkShaderModule shader_module)
{
  vkDestroyShaderModule (device, shader_module, NULL);
}

VkPipeline
create_compute_pipeline (VkDevice dev, VkShaderModule shader, 
  int sl_count, VkDescriptorSetLayout ds_layout[sl_count], 
  VkPipelineLayout * p_ppl_lay)
{
  VkPipelineLayout pl;
  {
    VkPipelineLayoutCreateInfo plci = {0};
    plci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    plci.setLayoutCount = sl_count;
    plci.pSetLayouts = ds_layout;
    plci.pushConstantRangeCount = 0;
    plci.pPushConstantRanges = NULL;
    if (vkCreatePipelineLayout (dev, &plci, NULL, &pl) != VK_SUCCESS)
      errx (-1, "(my)vkCreatePipelineLayout\n");
  }
  *p_ppl_lay = pl;

  VkPipeline p;
  {

    VkPipelineShaderStageCreateInfo pssci = {0};
    pssci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pssci.module = shader;
    pssci.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pssci.pName = "main";

    VkComputePipelineCreateInfo cpci = {0};
    cpci.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    cpci.basePipelineHandle = VK_NULL_HANDLE; //these two members are used for inherit
    cpci.basePipelineIndex = -1;
    cpci.stage = pssci;
    cpci.layout = pl;

    if (vkCreateComputePipelines (dev, VK_NULL_HANDLE, 1, &cpci, NULL, &p) != VK_SUCCESS)
      errx (-1, "(my)vkCreateComputePipelines\n");
  }
  return p;
}

VkDescriptorSetLayout
create_descriptor_set_layout (VkDevice dev, int binding_count, uint32_t binding[binding_count])
{
  VkDescriptorSetLayout ds_lay;
  {
    VkDescriptorSetLayoutBinding dslb[binding_count];
    for (int i = 0; i < binding_count; i++)
    {
      dslb[i].binding = binding[i];
      dslb[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
      dslb[i].descriptorCount = 1;
      dslb[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
      dslb[i].pImmutableSamplers = NULL;
    }

    VkDescriptorSetLayoutCreateInfo dslci = {0};
    dslci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dslci.bindingCount = binding_count;
    dslci.pBindings = dslb;

    if (vkCreateDescriptorSetLayout (dev, &dslci, NULL, &ds_lay) != VK_SUCCESS)
      errx (-1, "(my)vkCreateDescriptorSetLayout\n");
  }
  return ds_lay;
}

VkDescriptorPool
create_descriptor_pool (VkDevice dev, uint32_t size)
{
  VkDescriptorPool pool;
  {
    VkDescriptorPoolSize dps = {0};
    dps.descriptorCount = size;
    dps.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

    VkDescriptorPoolCreateInfo dpci = {0};
    dpci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    dpci.poolSizeCount = 1;
    dpci.pPoolSizes = &dps;
    dpci.maxSets = size;
    if (vkCreateDescriptorPool (dev, &dpci, NULL, &pool) != VK_SUCCESS)
      errx (-1, "(my)vkCreateDescriptorPool\n");
  }
  return pool;
}

VkDescriptorSet
create_descriptor_set (VkDevice dev, VkDescriptorPool pool, VkDescriptorSetLayout * pds_layout)
{
  int descriptor_count = 1;
  int max_sets = 1;

  VkDescriptorSet ds;
  {
    VkDescriptorSetAllocateInfo dsai = {0};
    dsai.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    dsai.descriptorPool = pool;
    dsai.descriptorSetCount = 1;
    dsai.pSetLayouts = pds_layout;
    if (vkAllocateDescriptorSets (dev, &dsai, &ds) != VK_SUCCESS)
      errx (-1, "(my)vkAllocateDescriptorSets\n");
  }
  return ds;
}

void descriptor_set_bind(VkDevice dev, VkDescriptorSet dstset,
                                VkBuffer buffer,
                                VkDeviceSize size,
                                uint32_t binding)
{
    VkDescriptorBufferInfo buffer_info = {
        buffer,
        0,
        size,
    };

    VkWriteDescriptorSet write_info = {
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        NULL,
        dstset,
        binding,
        0,
        1,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        NULL,
        &buffer_info,
        NULL
    };

    vkUpdateDescriptorSets(dev, 1, &write_info, 0, NULL);
}

void
add_buffer_to_descriptor_set (VkDevice dev, VkDescriptorSet dstset, int n, VkBuffer buf[n], size_t bufsize[n], int binding[n])
{
  VkDescriptorBufferInfo dbi[n];
  VkWriteDescriptorSet write[n];
  for (int i = 0; i < n; i++)
  {
    dbi[i].buffer = buf[i];
    dbi[i].offset = 0;
    dbi[i].range = bufsize[i];

    write[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write[i].descriptorCount = 1;
    write[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    write[i].dstBinding = binding[i];
    write[i].dstArrayElement = 0;
    write[i].dstSet = dstset;
    write[i].pBufferInfo = dbi + i;
  }
  vkUpdateDescriptorSets (dev, n, write, 0, NULL);
}


VkCommandPool
create_command_pool (VkDevice dev, uint32_t queue_fam_idx)
{
  VkCommandPool cmd_pool;
  {
    VkCommandPoolCreateInfo cpci = {0};
    cpci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cpci.queueFamilyIndex = queue_fam_idx;
    if (vkCreateCommandPool (dev, &cpci, NULL, &cmd_pool) != VK_SUCCESS)
      errx (-1, "(my)vkCreateCommandPoll\n");
  }
  return cmd_pool;
}

VkCommandBuffer
create_command_buffer (VkDevice dev, VkCommandPool cmd_pool)
{
  int cmd_buf_cnt = 1;

  VkCommandBuffer cmd_buf;
  {
    VkCommandBufferAllocateInfo cbai = {0};
    cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbai.commandPool = cmd_pool;
    cbai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cbai.commandBufferCount = cmd_buf_cnt;

    if (vkAllocateCommandBuffers (dev, &cbai, &cmd_buf) != VK_SUCCESS)
      errx (-1, "(my)vkAllocateCommandBuffers\n");
  }

  return cmd_buf;
}

void
record_command_buffer (VkCommandBuffer cmd_buf, 
  VkPipelineLayout ppl_layout, VkPipeline pipeline, int first_set, 
  uint32_t set_cnt, VkDescriptorSet * pds, int dispatch[3])
{

  {
    VkCommandBufferBeginInfo cbbi = {0};
    cbbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer (cmd_buf, &cbbi) != VK_SUCCESS)
      errx (-1, "(my)vkBeginCommandBuffer\n");
  }

  vkCmdBindPipeline (cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
  vkCmdBindDescriptorSets (cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE, ppl_layout, first_set, set_cnt, pds, 0, NULL);
//  vkCmdBindDescriptorSets (cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE, ppl_layout, 0, 1, pds, 0, NULL);

  vkCmdDispatch (cmd_buf, dispatch[0], dispatch[1], dispatch[2]);
//  vkCmdDispatch (cmd_buf, 1, 1, 1);

  vkEndCommandBuffer (cmd_buf);
}
