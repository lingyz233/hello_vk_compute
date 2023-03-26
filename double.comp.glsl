#version 450

layout (local_size_x = 256) in;

layout (set = 0, binding = 0) buffer StorageBuffer
{
  float data[];
} block;

void main ()
{
  uint gid = gl_GlobalInvocationID.x;
  block.data[gid] *= 2.0f;
}